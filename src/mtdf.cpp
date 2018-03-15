#include "mtdf.hpp"
#include "ZobristTable.hpp"
#include "Player.hpp"

bool            times_up(std::chrono::steady_clock::time_point start, uint32_t limit) { // THIS FUNCTION WORKS
    return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() > limit);
}

t_node          create_node(Player const& player, Player const& opponent) {
    t_node  node;

    node.player = player.board;
    node.opponent = opponent.board;
    node.player_forbidden = player.board_forbidden;
    node.opponent_forbidden = opponent.board_forbidden;
    node.pid = 1;
    node.player_pairs_captured = player.get_pairs_captured();
    node.opponent_pairs_captured = opponent.get_pairs_captured();
    return (node);
}

BitBoard     moves_to_explore(BitBoard const& player, BitBoard const& opponent, BitBoard const& player_forbidden, int player_pairs_captured, int opponent_pairs_captured) {
    BitBoard    moves;
    BitBoard    tmp;
    bool        win_instant = false;
    bool        player_empty = player.is_empty();
    bool        opponent_empty = opponent.is_empty();

    if (player_empty) { /* if player has no stones */
        if (!opponent_empty) /* if opponent has stones */
            moves |= get_moves_to_explore(opponent, player);
        else /* if the board is totally empty */
            moves.write(9, 9);
        return (moves);
    }
    /* if five aligned, we want to play the counter move */
    if (detect_five_aligned(opponent)) {
        moves = pair_capture_breaking_five_detector(player, opponent);
        if (moves.is_empty())
            moves = highlight_win_capture_moves(player, opponent, player_pairs_captured);
        if (!moves.is_empty()) // protection
            return (moves & ~player & ~opponent);
    }

    moves |= get_winning_moves(player, opponent, player_pairs_captured);
    if (!moves.is_empty()) win_instant = true;
    moves |= get_threat_moves(player, opponent, opponent_pairs_captured);
    // moves |= pair_capture_detector(player, opponent);
    moves |= pair_capture_detector(opponent, player);
    moves |= pattern_detector_highlight_open(opponent, player, { 0x60, 4, 4, 0 }); // -OO-, threatening capture of opponent stones
    if (!win_instant)
        moves |= pair_capture_detector(player, opponent);

    if (moves.set_count() <= EXPLORATION_THRESHOLD && !win_instant) {
        moves |= future_pattern_detector(player, opponent, { 0x78, 6, 4, 0 }); // -OOOO-
        if (moves.set_count() <= EXPLORATION_THRESHOLD) {
            moves |= future_pattern_detector(player, opponent, { 0x70, 5, 4, 0 }); // -OOO-
            moves |= future_pattern_detector(player, opponent, { 0x68, 6, 8, 0 }); // -OO-O-
            moves |= future_pattern_detector(player, opponent, { 0xF0, 5, 8, 0 }); // |OOOO- // ??
            // NOTE: if all moves to explore are threatened by capture, maybe explore other moves (it happens yeah)
            if (moves.is_empty())
                moves |= get_moves_to_explore(player, opponent) & ~player_forbidden;
        }
    }
    return (moves & ~player & ~opponent & ~player_forbidden);
}

t_ret   alphaBetaWithMemory(t_node node, int alpha, int beta, int depth) {
    return (alphaBetaMax(node, alpha, beta, depth));
}

t_ret   alphaBetaMin(t_node node, int alpha, int beta, int depth) {
    t_ret   ret;
    t_ret   best = {INF, 0 };

    if (depth == 0 || check_end(node))
        return ((t_ret){ score_function(node, depth+1), 0 });

    BitBoard    moves = moves_to_explore(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured, node.player_pairs_captured);
    for (int i = 0; i < 361; ++i) {
        if (moves.check_bit(i)) {
            ret = alphaBetaMax(simulate_move(node, i), alpha, beta, depth-1);
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

t_ret   alphaBetaMax(t_node node, int alpha, int beta, int depth) {
    t_ret   ret;
    t_ret   best = {-INF, 0 };

    if (depth == 0 || check_end(node))
        return ((t_ret){ score_function(node, depth+1), 0 });

    BitBoard    moves = moves_to_explore(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, node.opponent_pairs_captured);
    for (int i = 0; i < 361; ++i) {
        if (moves.check_bit(i)) {
            ret = alphaBetaMin(simulate_move(node, i), alpha, beta, depth-1);
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

void        TT_store(t_node const &node, int32_t g, int32_t alpha, int32_t beta, int8_t depth, int flag) {
    t_stored    entry;

    entry.depth = depth;
    entry.score = g;
    entry.flag = flag;
    ZobristTable::map[{node.player, node.opponent}] = entry;
}

int32_t        TT_lookup(t_node const &node, int32_t alpha, int32_t beta, int8_t depth) {
    t_stored    entry;

    if (ZobristTable::map.count({node.player, node.opponent})) {
        entry = ZobristTable::map[{node.player, node.opponent}];
        if (entry.depth >= depth) {
            if (entry.flag == ZobristTable::flag::exact)
                return (entry.score);
            else if (entry.flag == ZobristTable::flag::alpha && entry.score <= alpha)
                return (alpha);
            else if (entry.flag == ZobristTable::flag::beta && entry.score >= beta)
                return (beta);
        }
    }
    return (-INF);
}

Eigen::Array2i  iterative_deepening(t_node *root, int8_t max_depth) { // THIS FUNCTION WORKS
    std::chrono::steady_clock::time_point   start = std::chrono::steady_clock::now();
    t_ret                                   ret = { 0, 0 };
    int tmp = 0;

    std::cout << std::endl;
    for (int depth = 1; depth < max_depth; depth++) {
        ret = mtdf(root, tmp, depth);
        tmp = (depth % 2 ? ret.score : tmp);
        // ret = alphaBetaWithMemory(*root, -INF, INF, depth);
        std::cout << ret.score << std::endl;
        if (times_up(start, 500)) {
            std::cout << "TIMES UP, reached depth : " << depth << std::endl;
            break;
        }
    }
    std::cout << "score: " << ret.score << ", pos: (" << ret.p / 19 << ", " << ret.p % 19 << ")\n";
    return ((Eigen::Array2i){ ret.p / 19, ret.p % 19 });
}

t_ret   mtdf(t_node *root, int32_t firstguess, int8_t depth) { // THIS FUNCTION WORKS?
    int32_t bound[2] = { -INF, INF };
    int32_t beta;
    t_ret   ret = { firstguess, 0 };

    do {
        beta = ret.score + (ret.score == bound[0]);
        ret = alphaBetaWithMemory(*root, beta-1, beta, depth);
        std::cout << "score: " << ret.score << ", pos: (" << ret.p / 19 << ", " << ret.p % 19 << ")\n";
        bound[(ret.score < beta)] = ret.score;
    } while (bound[0] >= bound[1]);
    return (ret);
}

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

t_node  simulate_move(t_node const &node, int i) { // this is the biggest performance hit
    t_node  child = node;
    /* simulate player move */
    if (child.pid == 1) {
        child.player.write(i);
        BitBoard captured = highlight_captured_stones(child.player, child.opponent, i);
        if (!captured.is_empty()) {
            child.player_pairs_captured += captured.set_count() / 2;
            child.opponent &= ~captured;
        }
        child.player_forbidden = forbidden_detector(child.player, child.opponent);
        child.opponent_forbidden = forbidden_detector(child.opponent, child.player);
        child.pid = 2;
    }/* simulate opponent move */
    else {
        child.opponent.write(i);
        BitBoard captured = highlight_captured_stones(child.opponent, child.player, i);
        if (!captured.is_empty()) {
            child.opponent_pairs_captured += captured.set_count() / 2;
            child.player &= ~captured;
        }
        child.player_forbidden = forbidden_detector(child.player, child.opponent);
        child.opponent_forbidden = forbidden_detector(child.opponent, child.player);
        child.pid = 1;
    }
    return (child);
}

int32_t        score_function(t_node const &node, uint8_t depth) {
    int32_t     score = 0;

    score += player_score(node, depth);
    score -= opponent_score(node, depth) * 2;
    return (score);
}

int32_t    player_score(t_node const &node, uint8_t depth) {
    BitBoard    board;
    int32_t     score = 0;

    if (detect_five_aligned(node.player))
        return (214748364 * depth);
    if (node.player_pairs_captured >= 5)
        return (214748364 * depth);
    for (int i = 0; i < 10; ++i) {
        board = pattern_detector(node.player, node.opponent, BitBoard::patterns[i]);
        score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value : 0);
    }
    score += pair_capture_detector(node.player, node.opponent).set_count() * 5000;// pairs capture threatening is good
    score += node.player_pairs_captured * node.opponent_pairs_captured * 10000;
    return (score);
}

int32_t    opponent_score(t_node const &node, uint8_t depth) {
    BitBoard    board;
    int32_t     score = 0;

    if (detect_five_aligned(node.opponent))
        return (214748364 * depth);
    if (node.opponent_pairs_captured >= 5)
        return (214748364 * depth);
    for (int i = 0; i < 10; ++i) {
        board = pattern_detector(node.opponent, node.player, BitBoard::patterns[i]);
        if (0 <= i && i <= 1)
            score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value * 5 : 0);
        else
            score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value : 0);
    }
    score += pair_capture_detector(node.opponent, node.player).set_count() * 5000;// pairs capture threatening is good
    score += node.opponent_pairs_captured * node.opponent_pairs_captured * 10000; // 10,000pts 20,000pts 40,000pts 80,000pts
    return (score);
}

bool check_end(t_node const& node) {
    if (node.pid == 1)
        return (check_end(node.player, node.opponent, node.player_pairs_captured, node.opponent_pairs_captured, 1));
    return (check_end(node.opponent, node.player, node.opponent_pairs_captured, node.player_pairs_captured, 2));
}

// Eigen::Array2i  iterativeDeepening(t_node root, int max_depth) {
//     AlphaBeta   alphabeta(max_depth, true, 500);
//     t_ret       ret = { 0, 0 };
//     t_ret       tmp = { 0, 0 };
//
//     std::cout << std::endl;
//     for (int depth = 1; depth < max_depth; depth++) {
//         tmp = alphabeta(root, -INF, INF, depth);
//         std::cout << tmp.score << std::endl;
//         if (alphabeta.search_stopped) {
//             std::cout << "TIMES UP, reached depth : " << depth-1 << std::endl;
//             break;
//         }
//         ret = tmp;
//     }
//     std::cout << "score: " << ret.score << ", pos: (" << ret.p / 19 << ", " << ret.p % 19 << ")\n";
//     return ((Eigen::Array2i){ ret.p / 19, ret.p % 19 });
// }
//
// AlphaBeta::AlphaBeta(int max_depth, bool use_time_limit, int time_limit) : _max_depth(max_depth), _use_search_limit(use_time_limit), _search_limit_ms(time_limit) {
//     this->_search_start = std::chrono::steady_clock::now();
//     this->search_stopped = false;
// }
//
// AlphaBeta::AlphaBeta(AlphaBeta const &src) {
//     *this = src;
// }
//
// AlphaBeta::~AlphaBeta(void) {
// }
//
// AlphaBeta	&AlphaBeta::operator=(AlphaBeta const &rhs) {
//     this->_max_depth = rhs.get_max_depth();
//     this->_search_limit_ms = rhs.get_search_limit_ms();
//     return (*this);
// }
//
// t_ret const AlphaBeta::operator()(t_node root, int alpha, int beta, int depth) {
//     // this->_search_start = std::chrono::steady_clock::now();
//
//     BitBoard    moves = get_moves_to_explore(root.player, root.opponent) & ~root.player_forbidden;
//     if (moves.set_count() == 0) {
//         moves = get_moves_to_explore(root.opponent, root.player) & ~root.opponent_forbidden;
//         if (moves.set_count() == 0)
//             moves.write(9, 9);
//     }
//
//     int32_t         v;
//     uint16_t        pos;
//
//     for (int m = 0; m < 361; ++m) {
//         if (moves.check_bit(m)) {
//             v = _min(_create_child(root, m), alpha, beta, depth-1);
//             if (v > alpha) {
//                 alpha = v;
//                 pos = m;
//             }
//         }
//     }
//     return ((t_ret){ alpha, pos });
// }
//
// int         AlphaBeta::_min(t_node const node, int alpha, int beta, int depth) {
//     if (this->_use_search_limit && this->_times_up())
//         return (-INF);
//     if (depth == 0 || check_end(node))
//     // if (depth == 0 || check_end(node.player, node.opponent, node.player_pairs_captured, node.opponent_pairs_captured))
//         return (score_function(node, depth+1));
//
//     BitBoard    moves = moves_to_explore(node.opponent, node.player, node.opponent_forbidden, node.player_forbidden, node.opponent_pairs_captured, node.player_pairs_captured) & ~node.opponent_forbidden; // get_moves_to_explore
//     // BitBoard    moves = get_moves_to_explore(node.opponent, node.player) & ~node.opponent_forbidden;
//
//     for (int m = 0; m < 361; ++m) {
//         if (moves.check_bit(m)) {
//             beta = min_val(beta, _max(_create_child(node, m), alpha, beta, depth-1));
//             if (beta <= alpha)
//                 break;
//         }
//     }
//     return (beta);
// }
//
// int         AlphaBeta::_max(t_node const node, int alpha, int beta, int depth) {
//     if (this->_use_search_limit && this->_times_up())
//         return (INF);
//     if (depth == 0 || check_end(node))
//     // if (depth == 0 || check_end(node.opponent, node.player, node.opponent_pairs_captured, node.player_pairs_captured))
//         return (score_function(node, depth+1));
//
//     BitBoard    moves = moves_to_explore(node.player, node.opponent, node.player_forbidden, node.opponent_forbidden, node.player_pairs_captured, node.opponent_pairs_captured) & ~node.player_forbidden; // get_moves_to_explore
//     // BitBoard    moves = get_moves_to_explore(node.player, node.opponent) & ~node.player_forbidden;
//
//     for (int m = 0; m < 361; ++m) {
//         if (moves.check_bit(m)) {
//             alpha = max_val(alpha, _min(_create_child(node, m), alpha, beta, depth-1));
//             if (beta <= alpha)
//                 break;
//         }
//     }
//     return (alpha);
// }
//
// t_node      AlphaBeta::_create_child(t_node const& parent, int m) {
//     t_node  child = parent;
//     /* simulate player move */
//     if (child.pid == 1) {
//         child.player.write(m);
//         BitBoard captured = highlight_captured_stones(child.player, child.opponent);
//         if (!captured.is_empty()) {
//             child.player_pairs_captured += captured.set_count() / 2;
//             child.opponent &= ~captured;
//         }
//         child.player_forbidden = forbidden_detector(child.player, child.opponent);
//         child.opponent_forbidden = forbidden_detector(child.opponent, child.player);
//         child.pid = 2;
//     }/* simulate opponent move */
//     else {
//         child.opponent.write(m);
//         BitBoard captured = highlight_captured_stones(child.opponent, child.player);
//         if (!captured.is_empty()) {
//             child.opponent_pairs_captured += captured.set_count() / 2;
//             child.player &= ~captured;
//         }
//         child.player_forbidden = forbidden_detector(child.player, child.opponent);
//         child.opponent_forbidden = forbidden_detector(child.opponent, child.player);
//         child.pid = 1;
//     }
//     return (child);
// }
//
// bool    AlphaBeta::_times_up(void) {
//     if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->_search_start).count() > this->_search_limit_ms) {
//         this->search_stopped = true;
//         return (true);
//     }
//     return (false);
// }

t_ret           max(t_ret const& a, t_ret const& b) { return (a.score > b.score ? a : b); };
t_ret           min(t_ret const& a, t_ret const& b) { return (a.score < b.score ? a : b); };
int             max(int const& a, t_ret const& b)   { return (a > b.score ? a : b.score); };
int             min(int const& a, t_ret const& b)   { return (a < b.score ? a : b.score); };
int             max(int const& a, int const& b)     { return (a > b ? a : b); };
int             min(int const& a, int const& b)     { return (a < b ? a : b); };
