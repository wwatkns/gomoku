#include "mtdf.hpp"
#include "ZobristTable.hpp"
#include "Player.hpp"

bool            times_up(std::chrono::steady_clock::time_point start, uint32_t limit) {
    return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() > limit);
}

t_node          create_node(Player const& player, Player const& opponent) {
    t_node  node;

    node.player = player.board;
    node.opponent = opponent.board;
    node.player_forbidden = player.board_forbidden;
    node.opponent_forbidden = opponent.board_forbidden;
    node.cid = 1;
    node.player_pairs_captured = player.get_pairs_captured();
    node.opponent_pairs_captured = opponent.get_pairs_captured();
    return (node);
}

BitBoard     moves_to_explore(BitBoard const& player, BitBoard const& opponent, BitBoard const& player_forbidden, int player_pairs_captured, int opponent_pairs_captured) {
    BitBoard    moves;

    /* game start conditions */
    if (player.is_empty()) { /* if player has no stones */
        if (!opponent.is_empty()) /* if opponent has stones */
            moves |= opponent.dilated() & ~player; /* dilate around opponent */
        else /* if the board is totally empty */
            moves.write(9, 9);
        return (moves & ~player & ~opponent);
    }

    /* if five aligned, we want to play the counter move */
    if (detect_five_aligned(opponent)) {
        moves = pair_capture_breaking_five_detector(player, opponent);
        if (moves.is_empty())
            moves = win_by_capture_detector(player, opponent, player_pairs_captured);
        if (!moves.is_empty())
            return (moves & ~player & ~opponent);
    }

    /* if we detect an instant winning move, we return only this one */
    moves |= win_by_capture_detector(player, opponent, player_pairs_captured);
    moves |= win_by_alignment_detector(player, opponent, player_forbidden, opponent_pairs_captured);
    if (!moves.is_empty())
        return (moves & ~player & ~opponent & ~player_forbidden);

    /* opponent threats of open-threes, five-alignments and captures */
    moves |= get_threat_moves(player, opponent, opponent_pairs_captured);
    moves |= pair_capture_detector(opponent, player);

    /* explore building fives (non-instant win), open-fours, pair captures and opponent stone capture threats */
    moves |= future_pattern_detector(player, opponent, { 0xF8, 5, 8, 0 }); // OOOOO
    moves |= future_pattern_detector(player, opponent, { 0x78, 6, 4, 0 }); // -OOOO-
    moves |= pair_capture_detector(player, opponent);
    // moves |= pattern_detector_highlight_open(opponent, player, { 0x60, 4, 4, 0 }); // -OO-, threatening capture of opponent stones

    /* explore building open-threes */
    if (moves.set_count() <= EXPLORATION_THRESHOLD) {
        moves |= pattern_detector_highlight_open(opponent, player, { 0x60, 4, 4, 0 }); // -OO-, threatening capture of opponent stones
        moves |= future_pattern_detector(player, opponent, { 0x70, 5, 4, 0 }); // -OOO-
        moves |= future_pattern_detector(player, opponent, { 0x68, 6, 8, 0 }); // -OO-O-

        /* explore building close-four (to delay by one turn), is it necessary ? seems a bit like a bitch move */
        if (moves.set_count() <= EXPLORATION_THRESHOLD) {
            moves |= future_pattern_detector(player, opponent, { 0xF0, 5, 8, 0 }); // |OOOO-
            if (moves.is_empty())
                moves |= player.dilated() & ~opponent; /* dilate around player */
        }
    }
    return (moves & ~player & ~opponent & ~player_forbidden);
}

// static void debug_scores(int score, int i, int depth, int max_depth = 5) {
//     if (depth == max_depth)
//         std::printf("  %2d-%c : %d\n", 19-(i/19), "ABCDEFGHJKLMNOPQRST"[i%19], score);
// }

// t_ret   alphaBetaWithMemory(t_node node, int alpha, int beta, int depth) {
//     printf("> player%d explored moves:\n", node.cid);
//     return (alphaBetaMax(node, alpha, beta, depth));
// }
//
// t_ret   alphaBetaMin(t_node node, int alpha, int beta, int depth) {
//     t_ret   ret;
//     t_ret   best = {INF, 0 };
//
//     if (depth == 0 || check_end(node))
//         return ((t_ret){ score_function(node, depth+1), 0 });
//
//     BitBoard    moves = moves_to_explore(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured, node.player_pairs_captured);
//     for (int i = 0; i < 361; ++i) {
//         if (moves.check_bit(i)) {
//             ret = alphaBetaMax(simulate_move(node, i), alpha, beta, depth-1);
//             if (ret.score < best.score) {
//                 best = { ret.score, i };
//                 beta = min(beta, best.score);
//                 if (alpha >= beta) /* beta cut-off */
//                     break;
//             }
//         }
//     }
//     return (best);
// }

// t_ret   alphaBetaMax(t_node node, int alpha, int beta, int depth) {
//     t_ret   ret;
//     t_ret   best = {-INF, 0 };
//
//     if (depth == 0 || check_end(node))
//         return ((t_ret){ score_function(node, depth+1), 0 });
//
//     BitBoard    moves = moves_to_explore(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, node.opponent_pairs_captured);
//     for (int i = 0; i < 361; ++i) {
//         if (moves.check_bit(i)) {
//             ret = alphaBetaMin(simulate_move(node, i), alpha, beta, depth-1);
//             debug_scores(ret.score, i, depth);
//             if (ret.score > best.score) {
//                 best = { ret.score, i };
//                 alpha = max(alpha, best.score);
//                 if (alpha >= beta) /* alpha cut-off */
//                     break;
//             }
//         }
//     }
//     return (best);
// }

// void        TT_store(t_node const &node, int32_t g, int32_t alpha, int32_t beta, int8_t depth, int flag) {
//     t_stored    entry;
//
//     entry.depth = depth;
//     entry.score = g;
//     entry.flag = flag;
//     ZobristTable::map[{node.player, node.opponent}] = entry;
// }
//
// int32_t        TT_lookup(t_node const &node, int32_t alpha, int32_t beta, int8_t depth) {
//     t_stored    entry;
//
//     if (ZobristTable::map.count({node.player, node.opponent})) {
//         entry = ZobristTable::map[{node.player, node.opponent}];
//         if (entry.depth >= depth) {
//             if (entry.flag == ZobristTable::flag::exact)
//                 return (entry.score);
//             else if (entry.flag == ZobristTable::flag::alpha && entry.score <= alpha)
//                 return (alpha);
//             else if (entry.flag == ZobristTable::flag::beta && entry.score >= beta)
//                 return (beta);
//         }
//     }
//     return (-INF);
// }

// t_ret  iterative_deepening(t_node root, int8_t max_depth) { // THIS FUNCTION WORKS
//     std::chrono::steady_clock::time_point   start = std::chrono::steady_clock::now();
//     t_ret                                   ret = { 0, 0 };
//     t_ret                                   last;
//
//     std::cout << std::endl;
//     for (int depth = 1; depth < max_depth; depth += 2) {
//         ret = alphaBetaWithMemory(root, -INF, INF, depth);
//         std::cout << ret.score << std::endl;
//         if (times_up(start, 500)) {
//             ret = last;
//             std::cout << "TIMES UP, reached depth : " << depth << std::endl;
//             break;
//         }
//         last = ret;
//     }
//     return (ret);
// }
//
// t_ret   mtdf(t_node *root, int32_t firstguess, int8_t depth) { // THIS FUNCTION WORKS?
//     int32_t bound[2] = { -INF, INF };
//     int32_t beta;
//     t_ret   ret = { firstguess, 0 };
//
//     do {
//         beta = ret.score + (ret.score == bound[0]);
//         ret = alphaBetaWithMemory(*root, beta-1, beta, depth);
//         std::cout << "score: " << ret.score << ", pos: (" << ret.p / 19 << ", " << ret.p % 19 << ")\n";
//         bound[(ret.score < beta)] = ret.score;
//     } while (bound[0] < bound[1]);
//     return (ret);
// }

// void        TT_store(t_node const &node, int32_t best, int32_t alpha, int32_t beta, int8_t depth) {
//     t_stored    entry;
//
//     entry.depth = depth;
//     entry.score = best;
//     if (best <= alpha)
//         entry.flag = ZobristTable::flag::upperbound;
//     else if (best >= beta)
//         entry.flag = ZobristTable::flag::lowerbound;
//     else
//         entry.flag = ZobristTable::flag::exact;
//     ZobristTable::map[{node.player, node.opponent}] = entry;
//     // std::cout << "TT storing : " << entry.score << " " << std::to_string(entry.flag) << " " << std::to_string(entry.depth) << std::endl;
// }

// int32_t        TT_lookup(t_node const &node, int32_t alpha, int32_t beta, int8_t depth) {
//     t_stored    entry;
//
//     if (ZobristTable::map.count({node.player, node.opponent})) {
//         entry = ZobristTable::map[{node.player, node.opponent}];
//         // std::cout << "TT lookup : " << entry.score << " " << std::to_string(entry.flag) << " " << std::to_string(entry.depth) << std::endl;
//         // std::cout << "TT lookup at " << std::to_string(depth) << std::endl;
//         if (entry.depth >= depth) {
//             if (entry.flag == ZobristTable::flag::exact)
//                 return (entry.score);
//             else if (entry.flag == ZobristTable::flag::lowerbound && entry.score > alpha)
//                 alpha = entry.score;
//             else if (entry.flag == ZobristTable::flag::upperbound && entry.score < beta)
//                 beta = entry.score;
//             if (alpha >= beta)
//                 return (entry.score);
//         }
//     }
//     return (-INF);
// }

// t_node  simulate_move(t_node const &node, int i) { // this is the biggest performance hit
//     t_node  child = node;
//     /* simulate player move */
//     if (child.cid == 1) {
//         child.player.write(i);
//         BitBoard captured = highlight_captured_stones(child.player, child.opponent, i);
//         if (!captured.is_empty()) {
//             child.player_pairs_captured += captured.set_count() / 2;
//             child.opponent &= ~captured;
//         }
//         child.player_forbidden = forbidden_detector(child.player, child.opponent);
//         child.opponent_forbidden = forbidden_detector(child.opponent, child.player);
//         child.cid = 2;
//     }/* simulate opponent move */
//     else {
//         child.opponent.write(i);
//         BitBoard captured = highlight_captured_stones(child.opponent, child.player, i);
//         if (!captured.is_empty()) {
//             child.opponent_pairs_captured += captured.set_count() / 2;
//             child.player &= ~captured;
//         }
//         child.player_forbidden = forbidden_detector(child.player, child.opponent);
//         child.opponent_forbidden = forbidden_detector(child.opponent, child.player);
//         child.cid = 1;
//     }
//     return (child);
// }

AlphaBeta::AlphaBeta(int max_depth, int time_limit, uint8_t pid, uint8_t verbose) : _max_depth(max_depth), _current_max_depth(0), _search_limit_ms(time_limit), _pid(pid), _verbose(verbose) {
    this->search_stopped = false;
}

AlphaBeta::AlphaBeta(AlphaBeta const &src) {
    *this = src;
}

AlphaBeta::~AlphaBeta(void) {
}

AlphaBeta	&AlphaBeta::operator=(AlphaBeta const &rhs) {
    this->_max_depth = rhs.get_max_depth();
    this->_search_limit_ms = rhs.get_search_limit_ms();
    return (*this);
}

t_ret const AlphaBeta::operator()(t_node root) { /* iterative deepening */
    t_ret       ret = { 0, 0 };
    t_ret       tmp;
    this->search_stopped = false;
    this->_search_start = std::chrono::steady_clock::now();

    for (this->_current_max_depth = 1; this->_current_max_depth <= this->_max_depth; this->_current_max_depth += 2) { // we increment by 2 as scoring function is asymmetric
        tmp = _max(root, -INF, INF, this->_current_max_depth);
        _debug_search(tmp);
        if (this->search_stopped)
            break;
        ret = tmp;
    }
    return (ret);
}

t_ret   AlphaBeta::_min(t_node node, int alpha, int beta, int depth) {
    if (this->_times_up())
        return ((t_ret){-INF, 0 });

    if (depth == 0 || check_end(node))
        return ((t_ret){ score_function(node, depth+1), 0 });

    t_ret       ret;
    t_ret       best = {INF, 0 };
    BitBoard    moves = moves_to_explore(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured, node.player_pairs_captured);
    for (int i = 0; i < 361; ++i) {
        if (moves.check_bit(i)) {
            ret = _max(_create_child(node, i), alpha, beta, depth-1);
            if (ret.score < best.score) {
                best = { ret.score, i };
                beta = min(beta, best.score);
                if (alpha >= beta) /* beta cut-off */
                    break;
            }
        }
    }
    return (best);
}

t_ret   AlphaBeta::_max(t_node node, int alpha, int beta, int depth) {
    if (this->_times_up())
        return ((t_ret){ INF, 0 });

    if (depth == 0 || check_end(node))
        return ((t_ret){ score_function(node, depth+1), 0 });

    t_ret       ret;
    t_ret       best = {-INF, 0 };
    BitBoard    moves = moves_to_explore(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, node.opponent_pairs_captured);
    for (int i = 0; i < 361; ++i) {
        if (moves.check_bit(i)) {
            ret = _min(_create_child(node, i), alpha, beta, depth-1);
            _debug_append_explored(ret.score, i, depth);
            if (ret.score > best.score) {
                best = { ret.score, i };
                alpha = max(alpha, best.score);
                if (alpha >= beta) /* alpha cut-off */
                    break;
            }
        }
    }
    return (best);
}

t_node      AlphaBeta::_create_child(t_node const& parent, int m) {
    t_node  child = parent;
    /* simulate player move */
    if (child.cid == 1) {
        child.player.write(m);
        BitBoard captured = highlight_captured_stones(child.player, child.opponent, m);
        if (!captured.is_empty()) {
            child.player_pairs_captured += captured.set_count() / 2;
            child.opponent &= ~captured;
        }
        child.cid = 2;
    }/* simulate opponent move */
    else {
        child.opponent.write(m);
        BitBoard captured = highlight_captured_stones(child.opponent, child.player, m);
        if (!captured.is_empty()) {
            child.opponent_pairs_captured += captured.set_count() / 2;
            child.player &= ~captured;
        }
        child.cid = 1;
    }
    child.player_forbidden = forbidden_detector(child.player, child.opponent);
    child.opponent_forbidden = forbidden_detector(child.opponent, child.player);
    return (child);
}

bool    AlphaBeta::_times_up(void) {
    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->_search_start).count() >= this->_search_limit_ms) {
        this->search_stopped = true;
        return (true);
    }
    return (false);
}

int     AlphaBeta::_elapsed_ms(void) {
    return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->_search_start).count());
}

void    AlphaBeta::_debug_append_explored(int score, int i, int depth) {
    if (this->_verbose >= verbose::normal && depth == this->_current_max_depth) {
        char    tmp[256];
        std::sprintf(tmp, "  | %2d-%c : %11d pts\n", 19-(i/19), "ABCDEFGHJKLMNOPQRST"[i%19], score);
        this->_debug_string.append(tmp);
    }
}

void    AlphaBeta::_debug_search(t_ret const& ret) {
    if (this->_verbose >= verbose::normal) {
        if (this->_current_max_depth == 1)
            std::printf("\n[player %d - %s]\n", this->_pid, (this->_pid == 1 ? "black" : "white"));

        std::printf("[%c] Depth %d: %2d-%c, %11d pts in %3dms\n%s",
            (this->search_stopped ? 'x' : 'o'),
            this->_current_max_depth, 19-(ret.p/19),
            "ABCDEFGHJKLMNOPQRST"[ret.p%19],
            ret.score,
            _elapsed_ms(),
            (this->_verbose == verbose::debug ? (this->search_stopped ? "" : this->_debug_string.c_str()) : "")
        );
        this->_debug_string.clear();
    }
}


bool check_end(t_node const& node) {
    if (node.cid == 1)
        return (check_end(node.player, node.opponent, node.player_pairs_captured, node.opponent_pairs_captured, 1));
    return (check_end(node.opponent, node.player, node.opponent_pairs_captured, node.player_pairs_captured, 2));
}

int32_t        score_function(t_node const &node, uint8_t depth) {
    int64_t     score = 0;

    score += player_score(node, depth);
    score -= opponent_score(node, depth) * 2;
    return ((int32_t)range(score, (int64_t)-INF, (int64_t)INF));
}

int64_t    player_score(t_node const &node, uint8_t depth) {
    BitBoard    board;
    int64_t     score = 0;

    if (detect_five_aligned(node.player) && pair_capture_breaking_five_detector(node.opponent, node.player).is_empty()) // not good
        return (50000000 * depth); // 50,000,000pts for win
    if (node.player_pairs_captured >= 5)
        return (50000000 * depth); // 50,000,000pts for win
    for (int i = 0; i < 10; ++i) {
        board = pattern_detector(node.player, node.opponent, BitBoard::patterns[i]);
        if (0 <= i && i <= 2)
            score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value * 5 : 0);
        else
            score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value : 0);
    }
    score += pair_capture_detector(node.player, node.opponent).set_count() * 5000;//  5,000pts / pairs capture threatening
    score += node.player_pairs_captured * node.player_pairs_captured * 10000;     // 10,000pts 40,000pts 90,000pts 160,000pts
    score += (detect_five_aligned(node.player) ? 2000000 * depth : 0);            // 2,000,000pts for non winning five-alignment
    return (score);
}

int64_t    opponent_score(t_node const &node, uint8_t depth) {
    BitBoard    board;
    int64_t     score = 0;

    if (detect_five_aligned(node.opponent) && pair_capture_breaking_five_detector(node.player, node.opponent).is_empty()) // not good
        return (50000000 * depth);
    if (node.opponent_pairs_captured >= 5)
        return (50000000 * depth);
    for (int i = 0; i < 10; ++i) {
        board = pattern_detector(node.opponent, node.player, BitBoard::patterns[i]);
        if (0 <= i && i <= 2)
            score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value * 5 : 0);
        else
            score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value : 0);
    }
    score += pair_capture_detector(node.opponent, node.player).set_count() * 5000;//  5,000pts / pairs capture threatening
    score += node.opponent_pairs_captured * node.opponent_pairs_captured * 10000; // 10,000pts 40,000pts 90,000pts 160,000pts
    score += (detect_five_aligned(node.opponent) ? 2000000 * depth : 0);          // 2,000,000pts for non winning five-alignment
    return (score);
}


t_ret           max(t_ret const& a, t_ret const& b) { return (a.score > b.score ? a : b); };
t_ret           min(t_ret const& a, t_ret const& b) { return (a.score < b.score ? a : b); };
int             max(int const& a, t_ret const& b)   { return (a > b.score ? a : b.score); };
int             min(int const& a, t_ret const& b)   { return (a < b.score ? a : b.score); };
int             max(int const& a, int const& b)     { return (a > b ? a : b); };
int             min(int const& a, int const& b)     { return (a < b ? a : b); };
int             range(int const& x, int const& min, int const& max) { return (x > max ? max : (x < min ? min : x)); };
int64_t         range(int64_t const& x, int64_t const& min, int64_t const& max) { return (x > max ? max : (x < min ? min : x)); };
