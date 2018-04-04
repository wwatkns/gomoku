#include "AIPlayer.hpp"
#include "Player.hpp"
#include "GameEngine.hpp"

AIPlayer::AIPlayer(int depth, uint8_t verbose) : _depth(depth), _verbose(verbose) {
}

AIPlayer::AIPlayer(AIPlayer const &src) {
    *this = src;
}

AIPlayer        &AIPlayer::operator=(AIPlayer const&) {
    return(*this);
}

t_node      create_node(Player const& player, Player const& opponent) {
    t_node  node;

    node.player = player.board;
    node.opponent = opponent.board;
    node.cid = 1;
    node.player_pairs_captured = player.get_pairs_captured();
    node.opponent_pairs_captured = opponent.get_pairs_captured();
    return (node);
}

BitBoard    get_moves(BitBoard const& player, BitBoard const& opponent, BitBoard const& player_forbidden, int player_pairs_captured, int opponent_pairs_captured) {
    BitBoard    moves;

    /* game start conditions */
    if (player.is_empty()) { /* if player has no stones */
        if (!opponent.is_empty()) /* if opponent has stones */
            moves |= opponent.dilated() & ~player; /* dilate around opponent */
        else /* if the board is totally empty */
            moves.write(9, 9);
        return (moves & ~player & ~opponent);
    }
    /* if opponent has five aligned, we want to play the counter move */
    if (detect_five_aligned(opponent)) {
        moves = pair_capture_breaking_five_detector(player, opponent);
        if (moves.is_empty())
            moves = win_by_capture_detector(player, opponent, player_pairs_captured);
        if (!moves.is_empty())
            return (moves & ~player & ~opponent);
    }
    /* if player can win this turn, return this */
    moves = get_winning_moves(player, opponent, player_pairs_captured, opponent_pairs_captured);
    if (!moves.is_empty())
        return (moves);

    /* if opponent can win next turn, only explore the moves that will try to prevent that */
    moves = get_winning_moves(opponent, player, opponent_pairs_captured, player_pairs_captured);
    if (!moves.is_empty())
        return (moves | future_pattern_detector(player, opponent, { 0xF8, 5, 8, 0, 0 })); // OOOOO

    /* opponent threats of open-threes, five-alignments and captures */
    moves |= get_threat_moves(player, opponent, opponent_pairs_captured);
    moves |= pair_capture_detector(opponent, player);

    /* explore building fives (non-instant win), open-fours, pair captures and opponent stone capture threats */
    moves |= future_pattern_detector(player, opponent, { 0xF8, 5, 8, 0, 0 }); // OOOOO
    moves |= future_pattern_detector(player, opponent, { 0x78, 6, 8, 0, 0 }); // -OOOO-
    moves |= pair_capture_detector(player, opponent);

    /* explore building open-threes */
    if (moves.set_count() <= 4) {
        moves |= pattern_detector_highlight_open(opponent, player, { 0x60, 4, 4, 0, 0 }); // -OO-, threatening capture of opponent stones
        moves |= future_pattern_detector(player, opponent, { 0x70, 5, 8, 0, 0 }); // -OOO-
        moves |= future_pattern_detector(player, opponent, { 0x68, 6, 8, 0, 0 }); // -OO-O-

        /* explore building close-four (to delay by one turn), is it necessary ? seems a bit like a bitch move */
        if (moves.set_count() <= 4) {
            moves |= future_pattern_detector(player, opponent, { 0xF0, 5, 8, 0, 0 }); // |OOOO-
            // if (moves.is_empty())
            if (moves.set_count() <= 2)
                moves |= player.dilated() & ~opponent; /* dilate around player */
        }
    }
    return (moves & ~player & ~opponent & ~player_forbidden);
}

t_node          AIPlayer::create_child(t_node const &parent, int i) {
    t_node  child = parent;
    child.move = i;
    /* simulate player move */
    if (child.cid == 1) {
        child.player.write(i);
        BitBoard captured = highlight_captured_stones(child.player, child.opponent, i);
        if (!captured.is_empty()) {
            child.player_pairs_captured += captured.set_count() / 2;
            child.opponent &= ~captured;
        }
        child.cid = 2;
    }/* simulate opponent move */
    else {
        child.opponent.write(i);
        BitBoard captured = highlight_captured_stones(child.opponent, child.player, i);
        if (!captured.is_empty()) {
            child.opponent_pairs_captured += captured.set_count() / 2;
            child.player &= ~captured;
        }
        child.cid = 1;
    }
    return (child);
}

static inline int   popcount64(uint64_t x) {
    x = x - ((x >> 1) & 0x5555555555555555);
    x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
    return (((x + (x >> 4)) & 0x0F0F0F0F0F0F0F0F) * 0x0101010101010101) >> 56;
}

std::vector<t_move> AIPlayer::move_generation(t_node const& node, int depth) {
    std::vector<t_move> serialized;
    BitBoard            moves;

    /* compute the moves to explore for the given player */
    if (node.cid == 1)
        moves = get_moves(node.player, node.opponent, forbidden_detector(node.player, node.opponent), node.player_pairs_captured, node.opponent_pairs_captured);
    else
        moves = get_moves(node.opponent, node.player, forbidden_detector(node.opponent, node.player), node.opponent_pairs_captured, node.player_pairs_captured);
    /* convert the bitboard of moves to a list of positions */
    for (int i = 0; i < NICB; ++i)
        if (moves.values[i]) do {
            int idx = 63 - popcount64((moves.values[i] & -moves.values[i]) - 1) + (BITS * i);
            t_node move = this->create_child(node, idx);
            serialized.push_back((t_move){ this->evaluation_function(move, depth), idx, move });
        } while (moves.values[i] &= moves.values[i] - 1);
    /* sort the elements in the list by score */
    std::sort(serialized.begin(), serialized.end(), (node.cid == 2 ? sort_ascending : sort_descending));
    return (serialized);
}

static inline int32_t   player_score(t_node const &node, uint8_t depth) {
    BitBoard    board;
    BitBoard    captb;
    int64_t     score = 0;
    int64_t     count;
    int         value;

    /* return a score for a win by alignment (unbreakable) */
    board = highlight_five_aligned(node.player ^ pair_capture_detector_highlight(node.opponent, node.player));
    if (!board.is_empty() && win_by_capture_detector(node.opponent, node.player, node.opponent_pairs_captured).is_empty())
        return (50000000 * depth);
    /* return a score for a win by capture, weighted with the depth at which the win is found */
    if (node.player_pairs_captured >= 5)
        return (50000000 * depth);
    /* three-four if they are not threatened by a capture are sure win in 2 extra moves */
    board = three_four_detector(node.player, node.opponent);
    score += (board.is_empty() == false ? board.set_count() * (node.cid == 2 ? 500 : 1000) : 0);
    /* four-four if they are not threatened by a capture are sure win in 1 extra move */
    board = four_four_detector(node.player, node.opponent);
    score += (board.is_empty() == false ? board.set_count() * (node.cid == 2 ? 750 : 1200) : 0);
    /* count the score for all the patterns we find, and apply penalty for those that are threatened by capture */
    for (int i = 0; i < 8; ++i) {
        value = (node.cid == 2 ? BitBoard::patterns[i].value_0 : BitBoard::patterns[i].value_1);
        board = pattern_detector(node.player, node.opponent, BitBoard::patterns[i]);
        captb = pattern_detector(pair_capture_detector_highlight(node.opponent, node.player) ^ node.player, node.opponent, BitBoard::patterns[i]);
        count = (captb.is_empty() == false ? captb.set_count() : 0);
        score += (int64_t)((board.set_count() - count) * value * 0.25 + count * value);
    }
    score += pair_capture_detector(node.player, node.opponent).set_count() * (node.cid == 2 ? 3 : 10);/* evaluate opponent pair threatening */
    score += node.player_pairs_captured * node.player_pairs_captured * 20;      /* evaluate the pairs captured [0, 100, 400, 900, 1600, 2500] */
    return (score);
}

static inline int32_t   opponent_score(t_node const &node, uint8_t depth) {
    BitBoard    board;
    BitBoard    captb;
    int64_t     score = 0;
    int64_t     count;
    int         value;

    /* return a score for a win by alignment (unbreakable) */
    board = highlight_five_aligned(node.opponent ^ pair_capture_detector_highlight(node.player, node.opponent));
    if (!board.is_empty() && win_by_capture_detector(node.player, node.opponent, node.player_pairs_captured).is_empty())
        return (50000000 * depth);
    /* return a score for a win by capture, weighted with the depth at which the win is found */
    if (node.opponent_pairs_captured >= 5)
        return (50000000 * depth);
    /* three-four if they are not threatened by a capture are sure win in 2 extra moves */
    board = three_four_detector(node.opponent, node.player);
    score += (board.is_empty() == false ? board.set_count() * (node.cid == 1 ? 500 : 1000) : 0);
    /* four-four if they are not threatened by a capture are sure win in 1 extra move */
    board = four_four_detector(node.opponent, node.player);
    score += (board.is_empty() == false ? board.set_count() * (node.cid == 1 ? 750 : 1200) : 0);
    /* count the score for all the patterns we find, and apply penalty for those that are threatened by capture */
    for (int i = 0; i < 8; ++i) {
        value = (node.cid == 1 ? BitBoard::patterns[i].value_0 : BitBoard::patterns[i].value_1);
        board = pattern_detector(node.opponent, node.player, BitBoard::patterns[i]);
        captb = pattern_detector(pair_capture_detector_highlight(node.player, node.opponent) ^ node.opponent, node.player, BitBoard::patterns[i]);
        count = (captb.is_empty() == false ? captb.set_count() : 0);
        score += (int64_t)((board.set_count() - count) * value * 0.25 + count * value);
    }
    score += pair_capture_detector(node.opponent, node.player).set_count() * (node.cid == 1 ? 3 : 10);/* evaluate opponent pair threatening */
    score += node.opponent_pairs_captured * node.opponent_pairs_captured * 20;      /* evaluate the pairs captured [0, 100, 400, 900, 1600, 2500] */
    return (score);
}

int32_t         AIPlayer::score_function(t_node const &node, uint8_t depth) {
    int64_t     score = 0;

    score += player_score(node, depth);
    score -= (int64_t)(opponent_score(node, depth) * 1.5); // we give more weight to defense
    return ((int32_t)range(score, (int64_t)-INF, (int64_t)INF));
}

static inline int64_t   player_evaluation(t_node const &node, uint8_t depth) {
    int64_t     score = 0;

    if (!pattern_detector(node.player, node.opponent, BitBoard::patterns[0]).is_empty())
        return (50000000 * depth);
    if (node.player_pairs_captured >= 5)
        return (50000000 * depth);
    score += pair_capture_detector(node.player, node.opponent).set_count() * 50;
    score += node.player_pairs_captured * node.player_pairs_captured * 100;
    return (score);
}

static inline int64_t   opponent_evaluation(t_node const &node, uint8_t depth) {
    int64_t     score = 0;

    if (!pattern_detector(node.opponent, node.player, BitBoard::patterns[0]).is_empty())
        return (50000000 * depth);
    if (node.opponent_pairs_captured >= 5)
        return (50000000 * depth);
    score += pair_capture_detector(node.opponent, node.player).set_count() * 50;
    score += node.opponent_pairs_captured * node.opponent_pairs_captured * 100;
    return (score);
}

int32_t AIPlayer::evaluation_function(t_node const &node, uint8_t depth) {
    int64_t     score = 0;

    /* we give more weight to the player whose turn is next */
    score += player_evaluation(node, depth) * (node.cid == 2 ? 2:1);
    score -= opponent_evaluation(node, depth) * (node.cid == 1 ? 2:1);
    return (score);
}

bool    AIPlayer::checkEnd(t_node const& node) {
    if (node.cid == 1)
        return (check_end(node.player, node.opponent, node.player_pairs_captured, node.opponent_pairs_captured, node.move));
    return (check_end(node.opponent, node.player, node.opponent_pairs_captured, node.player_pairs_captured, node.move));
}

// void    AIPlayer::debug_append_explored(int score, int i, int depth) {
//     if (this->_verbose >= verbose::normal && depth == this->_current_max_depth) {
//         char    tmp[256];
//         std::sprintf(tmp, "  | %2d-%c : %11d pts\n", 19-(i/19), "ABCDEFGHJKLMNOPQRST"[i%19], score);
//         this->_debug_string.append(tmp);
//     }
// }
//
// void    AIPlayer::debug_search(t_ret const& ret) {
//     if (this->_verbose >= verbose::normal) {
//         if (this->_current_max_depth == 1)
//             std::printf("\n[player %d - %s]\n", this->_pid, (this->_pid == 1 ? "black" : "white"));
//
//         std::printf("[%c] Depth %d: %2d-%c, %11d pts in %3dms\n%s",
//             (this->search_stopped ? 'x' : 'o'),
//             this->_current_max_depth, 19-(ret.p/19),
//             "ABCDEFGHJKLMNOPQRST"[ret.p%19],
//             ret.score,
//             _elapsed_ms(),
//             (this->_verbose == verbose::debug ? (this->search_stopped ? "" : this->_debug_string.c_str()) : "")
//         );
//         this->_debug_string.clear();
//     }
// }

bool    sort_ascending(t_move const& a, t_move const& b) {
    return (a.eval < b.eval);
}

bool    sort_descending(t_move const& a, t_move const& b) {
    return (a.eval > b.eval);
}

int     range(int const& x, int const& min, int const& max) {
    return (x > max ? max : (x < min ? min : x));
}

int64_t range(int64_t const& x, int64_t const& min, int64_t const& max) {
    return (x > max ? max : (x < min ? min : x));
}
