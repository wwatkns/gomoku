#include "mtdf.hpp"
#include "Player.hpp"

// bool            times_up(std::chrono::steady_clock::time_point start, uint32_t limit) {
//     return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() > limit);
// }

t_node          create_node(Player const& player, Player const& opponent) {
    t_node  node;

    node.player = player.board;
    node.opponent = opponent.board;
    // node.player_forbidden = player.board_forbidden;
    // node.opponent_forbidden = opponent.board_forbidden;
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

/* comparison functions to sort std::vector<t_move> */
static bool cmpMin(t_move const& a, t_move const& b) { return (a.eval < b.eval); }
static bool cmpMax(t_move const& a, t_move const& b) { return (a.eval > b.eval); }


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


// AlphaBeta::AlphaBeta(int max_depth, int time_limit, uint8_t pid, uint8_t verbose) : _max_depth(max_depth), _current_max_depth(0), _search_limit_ms(time_limit), _pid(pid), _verbose(verbose) {
//     this->search_stopped = false;
//     this->_n_explored_nodes = 0;
//     this->_n_retreived_nodes = 0;
//     this->_n_stored_nodes = 0;
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
// std::multiset<t_ret, retmincmp> AlphaBeta::_min_move_ordering(t_node const& node, int alpha, int beta, int depth) {
//     std::multiset<t_ret, retmincmp> ordered_moves;
//     int                             score;
//     t_node                          child;
//     BitBoard                        moves = moves_to_explore(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured, node.player_pairs_captured);
//
//     for (int i = 0; i < 361; ++i) {
//         if (moves.check_bit(i)) {
//             child = _create_child(node, i);
//             score = 0; // TMP
//             /* TT Lookup of iterative deepening last depth node score for estimation */
//             if (this->_TT.find({child.player, child.opponent}) != this->_TT.end()) {
//                 t_stored stored = this->_TT[{child.player, child.opponent}];
//                 if (stored.max_id_depth == this->_current_max_depth - 2 && stored.depth == (this->_current_max_depth - depth) + 1) {
//                     if (stored.flag == ZobristTable::flag::exact)
//                         score = stored.score;
//                     else if (stored.flag == ZobristTable::flag::upperbound && stored.score <= alpha)
//                         score = alpha;
//                     else if (stored.flag == ZobristTable::flag::lowerbound && stored.score >= beta)
//                         score = beta;
//                 }
//             }
//             else
//                 score = 0;//score_function(node, depth+1);
//             ordered_moves.insert((t_ret){ score, i });
//         }
//     }
//     return (ordered_moves);
// }
//
// std::multiset<t_ret, retmaxcmp> AlphaBeta::_max_move_ordering(t_node const& node, int alpha, int beta, int depth) {
//     std::multiset<t_ret, retmaxcmp> ordered_moves;
//     int                             score;
//     t_node                          child;
//     BitBoard                        moves = moves_to_explore(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, node.opponent_pairs_captured);
//
//     for (int i = 0; i < 361; ++i) {
//         if (moves.check_bit(i)) {
//             child = _create_child(node, i);
//             score = 0; // TMP
//             /* TT Lookup of iterative deepening last depth node score for estimation */
//             if (this->_TT.find({child.player, child.opponent}) != this->_TT.end()) {
//                 t_stored stored = this->_TT[{child.player, child.opponent}];
//                 if (stored.max_id_depth == this->_current_max_depth - 2 && stored.depth == (this->_current_max_depth - depth) + 1) {
//                     if (stored.flag == ZobristTable::flag::exact)
//                         score = stored.score;
//                     else if (stored.flag == ZobristTable::flag::upperbound && stored.score <= alpha)
//                         score = alpha;
//                     else if (stored.flag == ZobristTable::flag::lowerbound && stored.score >= beta)
//                         score = beta;
//                 }
//             }
//             else
//                 score = 0;//score_function(node, depth+1);
//             ordered_moves.insert((t_ret){ score, i });
//         }
//     }
//     return (ordered_moves);
// }
//
// t_ret const AlphaBeta::_mtdf(t_node root, int firstguess, int depth) {
//     int bound[2] = { -INF, INF };
//     int beta;
//     t_ret   ret = { firstguess, 0 };
//
//     do {
//         beta = ret.score + (ret.score == bound[0]);
//         ret = _root_max(root, beta-1, beta, depth);
//         // ret = _max(root, beta-1, beta, depth);
//         bound[(ret.score < beta)] = ret.score;
//     } while (bound[0] < bound[1]);
//     return (ret);
// }
//
// t_ret const AlphaBeta::operator()(t_node root) { /* iterative deepening */
//     t_ret       ret = { 0, 0 };
//     t_ret       tmp;
//     this->search_stopped = false;
//     this->_search_start = std::chrono::steady_clock::now();
//     this->_ordered_root_moves.clear();
//
//     for (this->_current_max_depth = 1; this->_current_max_depth <= this->_max_depth; this->_current_max_depth += 2) {
//         // this->_TT.clear();
//         // tmp = _root_max(root, -INF, INF, this->_current_max_depth);
//         tmp = _mtdf(root, ret.score, this->_current_max_depth);
//         _debug_search(tmp);
//         if (this->search_stopped)
//             break;
//         ret = tmp;
//     }
//     return (ret);
// }
//
// t_ret   AlphaBeta::_min(t_node node, int alpha, int beta, int depth) {
//     /* Search time is up, we stop the search */
//     if (this->_times_up())
//         return ((t_ret){-INF, 0 });
//     /* Transposition Table Lookup */
//     t_ret lookup = _TT_lookup(node, alpha, beta, depth);
//     if (lookup.score != -INF)
//         return (lookup);
//     /* Leaf node, evaluation */
//     if (depth == 0 || check_end(node)) {
//         t_ret ret = { score_function(node, depth+1), -1 };
//         _TT_store(node, ret, depth, ZobristTable::flag::exact); /* NEW */
//         return (ret);
//     }
//
//     t_ret       ret;
//     t_ret       best = { INF, -1 };
//     // BitBoard    moves = moves_to_explore(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured, node.player_pairs_captured);
//     // for (int i = 0; i < 361; ++i) {
//     //     if (moves.check_bit(i)) {
//     std::multiset<t_ret, retmincmp> ordered_moves = _min_move_ordering(node, alpha, beta, depth);
//     for (std::multiset<t_ret, retmincmp>::iterator it = ordered_moves.begin(); it != ordered_moves.end(); ++it) {
//         ret = _max(_create_child(node, it->p), alpha, beta, depth-1);
//         if (ret.p == -1 || ret.score < best.score) {
//             best = { ret.score, it->p };
//             beta = min(beta, best.score);
//             if (alpha >= beta) /* alpha cut-off */ {
//                 _TT_store(node, best, depth, ZobristTable::flag::lowerbound); /* NEW */
//                 return (best);
//             }
//         }
//         // }
//     }
//     // _TT_store(node, best, depth, ZobristTable::flag::lowerbound); /* NEW */
//     return (best);
// }
//
// t_ret   AlphaBeta::_max(t_node node, int alpha, int beta, int depth) {
//     /* Search time is up, we stop the search */
//     if (this->_times_up())
//         return ((t_ret){ INF, 0 });
//     /* Transposition Table Lookup */
//     t_ret lookup = _TT_lookup(node, alpha, beta, depth);
//     if (lookup.score != -INF)
//         return (lookup);
//     /* Leaf node, evaluation */
//     if (depth == 0 || check_end(node)) {
//         t_ret ret = { score_function(node, depth+1), -1 };
//         _TT_store(node, ret, depth, ZobristTable::flag::exact); /* NEW */
//         return (ret);
//     }
//
//     t_ret       ret;
//     t_ret       best = {-INF, -1 };
//     // BitBoard    moves = moves_to_explore(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, node.opponent_pairs_captured);
//     // for (int i = 0; i < 361; ++i) {
//     //     if (moves.check_bit(i)) {
//
//     std::multiset<t_ret, retmaxcmp> ordered_moves = _max_move_ordering(node, alpha, beta, depth);
//     for (std::multiset<t_ret, retmaxcmp>::iterator it = ordered_moves.begin(); it != ordered_moves.end(); ++it) {
//         ret = _min(_create_child(node, it->p), alpha, beta, depth-1);
//         _debug_append_explored(ret.score, it->p, depth);
//         if (ret.p == -1 || ret.score > best.score) {
//             best = { ret.score, it->p };
//             alpha = max(alpha, best.score);
//             if (alpha >= beta) /* beta cut-off */ {
//                 _TT_store(node, best, depth, ZobristTable::flag::upperbound); /* NEW */
//                 return (best);
//             }
//         }
//         // }
//     }
//     // _TT_store(node, best, depth, ZobristTable::flag::upperbound); /* NEW */
//     return (best);
// }
//
// t_ret   AlphaBeta::_root_max(t_node node, int alpha, int beta, int depth) {
//     t_ret       ret;
//     t_ret       best = {-INF, -1 };
//
//     /* if we're at the top of our iterative deepening function */
//     if (this->_ordered_root_moves.size() == 0) {
//         BitBoard    moves = moves_to_explore(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, node.opponent_pairs_captured);
//         for (int i = 0; i < 361; ++i) {
//             if (moves.check_bit(i)) {
//                 ret = _min(_create_child(node, i), alpha, beta, depth-1);
//                 this->_ordered_root_moves.insert((t_ret){ ret.score, i }); /* NEW */
//                 _debug_append_explored(ret.score, i, depth);
//                 if (ret.p == -1 || ret.score > best.score) {
//                     best = { ret.score, i };
//                     alpha = max(alpha, best.score);
//                     if (alpha >= beta) /* beta cut-off */
//                         break;
//                 }
//             }
//         }
//     }
//     else {
//         std::multiset<t_ret, retmaxcmp> ordered_moves;
//
//         for (std::multiset<t_ret, retmaxcmp>::iterator it = this->_ordered_root_moves.begin(); it != this->_ordered_root_moves.end(); ++it) {
//             ret = _min(_create_child(node, it->p), alpha, beta, depth-1);
//             ordered_moves.insert((t_ret){ ret.score, it->p }); /* NEW */
//             _debug_append_explored(ret.score, it->p, depth);
//             if (ret.p == -1 || ret.score > best.score) {
//                 best = { ret.score, it->p };
//                 alpha = max(alpha, best.score);
//                 if (alpha >= beta) /* beta cut-off */
//                     break;
//             }
//         }
//         this->_ordered_root_moves = ordered_moves; /* NEW */
//     }
//     return (best);
// }
//
// t_ret       AlphaBeta::_TT_lookup(t_node const& node, int alpha, int beta, int8_t depth) {
//      /* the entry exists */
//     if (this->_TT.find({node.player, node.opponent}) != this->_TT.end()) {
//         int true_depth = (this->_current_max_depth - depth) + 1;
//         t_stored stored = this->_TT[{node.player, node.opponent}];
//         if (stored.max_id_depth == this->_current_max_depth && stored.depth <= true_depth) {
//             this->_n_retreived_nodes++;
//             // std::cout << "TT_lookup " << std::to_string(true_depth) << " - score: " << stored.score << ", d: " << std::to_string(stored.depth) << ", flag: " << std::to_string(stored.flag) << std::endl;
//             if (stored.flag == ZobristTable::flag::exact)
//                 return ((t_ret){ stored.score, stored.move });
//             else if (stored.flag == ZobristTable::flag::upperbound && stored.score <= alpha)
//                 return ((t_ret){ alpha, stored.move });
//             else if (stored.flag == ZobristTable::flag::lowerbound && stored.score >= beta)
//                 return ((t_ret){ beta, stored.move });
//         }
//     }
//     return ((t_ret){-INF, -1 });
// }
//
// void        AlphaBeta::_TT_store(t_node const& node, t_ret best, int8_t depth, int8_t flag) {
//     t_stored    entry;
//
//     entry.max_id_depth = this->_current_max_depth;
//     entry.depth = (this->_current_max_depth - depth) + 1;
//     entry.score = best.score;
//     entry.move  = best.p;
//     entry.flag  = flag;
//     this->_TT[{node.player, node.opponent}] = entry;
//     this->_n_stored_nodes++;
// }
//
// t_node      AlphaBeta::_create_child(t_node const& parent, int m) {
//     t_node  child = parent;
//     this->_n_explored_nodes++;
//     /* simulate player move */
//     if (child.cid == 1) {
//         child.player.write(m);
//         BitBoard captured = highlight_captured_stones(child.player, child.opponent, m);
//         if (!captured.is_empty()) {
//             child.player_pairs_captured += captured.set_count() / 2;
//             child.opponent &= ~captured;
//         }
//         child.cid = 2;
//     }/* simulate opponent move */
//     else {
//         child.opponent.write(m);
//         BitBoard captured = highlight_captured_stones(child.opponent, child.player, m);
//         if (!captured.is_empty()) {
//             child.opponent_pairs_captured += captured.set_count() / 2;
//             child.player &= ~captured;
//         }
//         child.cid = 1;
//     }
//     child.player_forbidden = forbidden_detector(child.player, child.opponent);
//     child.opponent_forbidden = forbidden_detector(child.opponent, child.player);
//     return (child);
// }
//
// bool    AlphaBeta::_times_up(void) {
//     if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->_search_start).count() >= this->_search_limit_ms) {
//         this->search_stopped = true;
//         return (true);
//     }
//     return (false);
// }
//
// int     AlphaBeta::_elapsed_ms(void) {
//     return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->_search_start).count());
// }
//
// void    AlphaBeta::_debug_append_explored(int score, int i, int depth) {
//     if (this->_verbose >= verbose::normal && depth == this->_current_max_depth) {
//         char    tmp[256];
//         std::sprintf(tmp, "  | %2d-%c : %11d pts\n", 19-(i/19), "ABCDEFGHJKLMNOPQRST"[i%19], score);
//         this->_debug_string.append(tmp);
//     }
// }
//
// void    AlphaBeta::_debug_search(t_ret const& ret) {
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
//
//         std::printf("  |  explored: %d\n  |    stored: %d\n  | retreived: %d\n",
//             this->_n_explored_nodes,
//             this->_n_stored_nodes,
//             this->_n_retreived_nodes
//         );
//         this->_n_explored_nodes = 0;
//         this->_n_retreived_nodes = 0;
//         this->_n_stored_nodes = 0;
//
//         this->_debug_string.clear();
//     }
// }



AlphaBetaWithIterativeDeepening::AlphaBetaWithIterativeDeepening(int max_depth, int time_limit, uint8_t pid, uint8_t verbose) : _max_depth(max_depth), _current_max_depth(0), _search_limit_ms(time_limit), _pid(pid), _verbose(verbose) {
    this->search_stopped = false;
}

AlphaBetaWithIterativeDeepening::AlphaBetaWithIterativeDeepening(AlphaBetaWithIterativeDeepening const &src) {
    *this = src;
}

AlphaBetaWithIterativeDeepening::~AlphaBetaWithIterativeDeepening(void) {
}

AlphaBetaWithIterativeDeepening	&AlphaBetaWithIterativeDeepening::operator=(AlphaBetaWithIterativeDeepening const &rhs) {
    this->_max_depth = rhs.get_max_depth();
    this->_search_limit_ms = rhs.get_search_limit_ms();
    return (*this);
}

t_ret const AlphaBetaWithIterativeDeepening::operator()(t_node root) { /* iterative deepening */
    t_ret       ret = { 0, 0 };
    t_ret       current;

    this->search_stopped = false;
    this->_search_start = std::chrono::steady_clock::now();
    this->_root_moves.clear();

    for (this->_current_max_depth = 1; this->_current_max_depth <= this->_max_depth; this->_current_max_depth += 2) {
        current = _root_max(root, -INF, INF, this->_current_max_depth);
        _debug_search(current);
        if (this->search_stopped)
            break;
        ret = current;
    }
    return (ret);
}

t_ret   AlphaBetaWithIterativeDeepening::_min(t_node node, int alpha, int beta, int depth) {
    /* do we exceed our maximum allowed search time */
    if (this->_times_up())
        return ((t_ret){-INF, 0 });
    /* is the node a leaf or the game is won */
    if (depth == 0 || check_end(node))
        return ((t_ret){ score_function(node, depth+1), 0 });

    t_ret               current;
    t_ret               best = { INF, 0 };
    std::vector<t_move> moves = _move_generation(node, depth);

    for (std::vector<t_move>::const_iterator move = moves.begin(); move != moves.end(); ++move) {
        current = _max(move->node, alpha, beta, depth-1);
        if (current < best) {
            best = { current.score, move->p };
            beta = min(beta, best.score);
            if (alpha >= beta) /* alpha cut-off */
                return (best);
        }
    }
    return (best);
}

t_ret   AlphaBetaWithIterativeDeepening::_max(t_node node, int alpha, int beta, int depth) {
    /* do we exceed our maximum allowed search time */
    if (this->_times_up())
        return ((t_ret){ INF, 0 });
    /* is the node a leaf or the game is won */
    if (depth == 0 || check_end(node))
        return ((t_ret){ score_function(node, depth+1), 0 });

    t_ret               current;
    t_ret               best = {-INF, 0 };
    std::vector<t_move> moves = _move_generation(node, depth);

    for (std::vector<t_move>::const_iterator move = moves.begin(); move != moves.end(); ++move) {
        current = _min(move->node, alpha, beta, depth-1);
        _debug_append_explored(current.score, move->p, depth);
        if (current > best) {
            best = { current.score, move->p };
            alpha = max(alpha, best.score);
            if (alpha >= beta) /* beta cut-off */
                return (best);
        }
    }
    return (best);
}

t_ret   AlphaBetaWithIterativeDeepening::_root_max(t_node node, int alpha, int beta, int depth) {
    t_ret       current;
    t_ret       best = {-INF, 0 };

    /* if we're at the top of our iterative deepening function */
    if (this->_current_max_depth == 1)
        this->_root_moves = _move_generation(node, depth);
    /* otherwise the estimation at the previous iterative deepening loop will be used */
    for (std::vector<t_move>::iterator move = this->_root_moves.begin(); move != this->_root_moves.end(); ++move) {
        current = _min(move->node, alpha, beta, depth-1);
        move->eval = current.score;
        _debug_append_explored(current.score, move->p, depth);
        if (current > best) {
            best = { current.score, move->p };
            alpha = max(alpha, best.score);
        }
    }
    std::sort(this->_root_moves.begin(), this->_root_moves.end(), cmpMax);
    return (best);
}

static int popcount64(uint64_t x) {
    x = x - ((x >> 1) & 0x5555555555555555);
    x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
    return (((x + (x >> 4)) & 0x0F0F0F0F0F0F0F0F) * 0x0101010101010101) >> 56;
}

// this will also take into account the estimation of the node at previous iterative deepening loop
std::vector<t_move> AlphaBetaWithIterativeDeepening::_move_generation(t_node const& node, int depth) {
    std::vector<t_move> serialized;
    BitBoard            moves;

    /* compute the moves to explore for the given player */
    if (node.cid == 1)
        moves = moves_to_explore(node.player, node.opponent, forbidden_detector(node.player, node.opponent), node.player_pairs_captured, node.opponent_pairs_captured);
    else
        moves = moves_to_explore(node.opponent, node.player, forbidden_detector(node.opponent, node.player), node.opponent_pairs_captured, node.player_pairs_captured);
    /* convert the bitboard of moves to a list of positions */
    for (int i = 0; i < N; ++i)
        if (moves.values[i]) do {
            int idx = 63 - popcount64((moves.values[i] & -moves.values[i]) - 1) + (BITS * i);
            t_node move = _create_child(node, idx);
            serialized.push_back((t_move){ evaluation_function(move, depth), idx, move });
        } while (moves.values[i] &= moves.values[i] - 1);
    /* sort the elements in the list by score */
    std::sort(serialized.begin(), serialized.end(), (node.cid == 2 ? cmpMin : cmpMax));
    return (serialized);
}

t_node      AlphaBetaWithIterativeDeepening::_create_child(t_node const& parent, int m) {
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
    return (child);
}

bool    AlphaBetaWithIterativeDeepening::_times_up(void) {
    // if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->_search_start).count() >= this->_search_limit_ms) {
    //     this->search_stopped = true;
    //     return (true);
    // }
    return (false);
}

int     AlphaBetaWithIterativeDeepening::_elapsed_ms(void) {
    return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->_search_start).count());
}

void    AlphaBetaWithIterativeDeepening::_debug_append_explored(int score, int i, int depth) {
    if (this->_verbose >= verbose::normal && depth == this->_current_max_depth) {
        char    tmp[256];
        std::sprintf(tmp, "  | %2d-%c : %11d pts\n", 19-(i/19), "ABCDEFGHJKLMNOPQRST"[i%19], score);
        this->_debug_string.append(tmp);
    }
}

void    AlphaBetaWithIterativeDeepening::_debug_search(t_ret const& ret) {
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



AlphaBetaWithMemory::AlphaBetaWithMemory(int max_depth, int time_limit, uint8_t pid, uint8_t verbose) : _max_depth(max_depth), _current_max_depth(0), _search_limit_ms(time_limit), _pid(pid), _verbose(verbose) {
    this->search_stopped = false;
}

AlphaBetaWithMemory::AlphaBetaWithMemory(AlphaBetaWithMemory const &src) {
    *this = src;
}

AlphaBetaWithMemory::~AlphaBetaWithMemory(void) {
}

AlphaBetaWithMemory	&AlphaBetaWithMemory::operator=(AlphaBetaWithMemory const &rhs) {
    this->_max_depth = rhs.get_max_depth();
    this->_search_limit_ms = rhs.get_search_limit_ms();
    return (*this);
}

t_ret const AlphaBetaWithMemory::operator()(t_node root) { /* iterative deepening */
    t_ret       ret = { 0, 0 };
    t_ret       current;

    this->search_stopped = false;
    this->_search_start = std::chrono::steady_clock::now();
    this->_ordered_root_moves.clear();
    // this->_TT.clear();

    for (this->_current_max_depth = 1; this->_current_max_depth <= this->_max_depth; this->_current_max_depth += 1) {
        current = _root_max(root, -INF, INF, this->_current_max_depth);
        _debug_search(current);
        if (this->search_stopped)
            break;
        ret = current;
    }
    return (ret);
}

t_ret   AlphaBetaWithMemory::_min(t_node node, int alpha, int beta, int depth) {
    /* do we exceed our maximum allowed search time */
    if (this->_times_up())
        return ((t_ret){-INF, 0 });
    /* Transposition Table Lookup */
    t_ret lookup = _TT_lookup(node, alpha, beta, depth);
    if (lookup.score != -INF)
        return (lookup);
    /* is the node a leaf or the game is won */
    if (depth == 0 || check_end(node)) {
        t_ret ret = { score_function(node, depth+1), -1 };
        _TT_store(node, ret, depth, ZobristTable::flag::exact);
        return (ret);
    }

    t_ret       current;
    t_ret       best = { INF, 0 };
    BitBoard    moves = moves_to_explore(node.opponent, node.player, forbidden_detector(node.opponent, node.player), node.opponent_pairs_captured, node.player_pairs_captured);

    for (int i = 0; i < 361; ++i) if (moves.check_bit(i)) {
        current = _max(_create_child(node, i), alpha, beta, depth-1);
        if (current.p == -1 || current < best) {
            best = { current.score, i };
            beta = min(beta, best.score);
            if (alpha >= beta) { /* alpha cut-off */
                _TT_store(node, best, depth, ZobristTable::flag::lowerbound);
                return (best);
            }
        }
    }
    _TT_store(node, best, depth, ZobristTable::flag::lowerbound);
    return (best);
}

t_ret   AlphaBetaWithMemory::_max(t_node node, int alpha, int beta, int depth) {
    /* do we exceed our maximum allowed search time */
    if (this->_times_up())
        return ((t_ret){ INF, 0 });
    /* Transposition Table Lookup */
    t_ret lookup = _TT_lookup(node, alpha, beta, depth);
    if (lookup.score != -INF)
        return (lookup);
    /* is the node a leaf or the game is won */
    if (depth == 0 || check_end(node)) {
        t_ret ret = { score_function(node, depth+1), -1 };
        _TT_store(node, ret, depth, ZobristTable::flag::exact);
        return (ret);
    }

    t_ret       current;
    t_ret       best = {-INF, 0 };
    BitBoard    moves = moves_to_explore(node.player, node.opponent, forbidden_detector(node.player, node.opponent), node.player_pairs_captured, node.opponent_pairs_captured);

    for (int i = 0; i < 361; ++i) if (moves.check_bit(i)) {
        current = _min(_create_child(node, i), alpha, beta, depth-1);
        _debug_append_explored(current.score, i, depth);
        if (current.p == -1 || current > best) {
            best = { current.score, i };
            alpha = max(alpha, best.score);
            if (alpha >= beta) { /* beta cut-off */
                _TT_store(node, best, depth, ZobristTable::flag::upperbound);
                return (best);
            }
        }
    }
    _TT_store(node, best, depth, ZobristTable::flag::upperbound);
    return (best);
}

t_ret   AlphaBetaWithMemory::_root_max(t_node node, int alpha, int beta, int depth) {
    t_ret       current;
    t_ret       best = {-INF, 0 };

    /* if we're at the top of our iterative deepening function */
    if (this->_ordered_root_moves.size() == 0) {
        BitBoard    moves = moves_to_explore(node.player, node.opponent, forbidden_detector(node.player, node.opponent), node.player_pairs_captured, node.opponent_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                current = _min(_create_child(node, i), alpha, beta, depth-1);
                this->_ordered_root_moves.insert((t_ret){ current.score, i });
                _debug_append_explored(current.score, i, depth);
                if (current > best) {
                    best = { current.score, i };
                    alpha = max(alpha, best.score);
                    if (alpha >= beta) /* beta cut-off */
                        return (best);
                }
            }
        }
    } else {
        std::multiset<t_ret, retmaxcmp> ordered_moves;

        for (std::multiset<t_ret, retmaxcmp>::iterator it = this->_ordered_root_moves.begin(); it != this->_ordered_root_moves.end(); ++it) {
            current = _min(_create_child(node, it->p), alpha, beta, depth-1);
            ordered_moves.insert((t_ret){ current.score, it->p });
            _debug_append_explored(current.score, it->p, depth);
            if (current > best) {
                best = { current.score, it->p };
                alpha = max(alpha, best.score);
                if (alpha >= beta) /* beta cut-off */
                    return (best);
            }
        }
        this->_ordered_root_moves = ordered_moves;
    }
    return (best);
}

t_ret       AlphaBetaWithMemory::_TT_lookup(t_node const& node, int alpha, int beta, int8_t depth) {
     /* the entry exists */
    if (this->_TT.find({node.player, node.opponent}) != this->_TT.end()) {
        int true_depth = (this->_current_max_depth - depth) + 1;
        t_stored stored = this->_TT[{node.player, node.opponent}];
        if (stored.max_id_depth == this->_current_max_depth && stored.depth <= true_depth) {
            this->_n_retreived_nodes++;
            if (stored.flag == ZobristTable::flag::exact)
                return ((t_ret){ stored.score, stored.move });
            else if (stored.flag == ZobristTable::flag::upperbound && stored.score <= alpha)
                return ((t_ret){ alpha, stored.move });
            else if (stored.flag == ZobristTable::flag::lowerbound && stored.score >= beta)
                return ((t_ret){ beta, stored.move });
        }
    }
    return ((t_ret){-INF, -1 });
}

void        AlphaBetaWithMemory::_TT_store(t_node const& node, t_ret best, int8_t depth, int8_t flag) {
    t_stored    entry;
    // work on updating the hash from the move done

    entry.max_id_depth = this->_current_max_depth;
    entry.depth = (this->_current_max_depth - depth) + 1;
    entry.score = best.score;
    entry.move  = best.p;
    entry.flag  = flag;
    this->_TT[{node.player, node.opponent}] = entry;
    this->_n_stored_nodes++;
}

t_node      AlphaBetaWithMemory::_create_child(t_node const& parent, int m) {
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
    return (child);
}

bool    AlphaBetaWithMemory::_times_up(void) {
    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->_search_start).count() >= this->_search_limit_ms) {
        this->search_stopped = true;
        return (true);
    }
    return (false);
}

int     AlphaBetaWithMemory::_elapsed_ms(void) {
    return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->_search_start).count());
}

void    AlphaBetaWithMemory::_debug_append_explored(int score, int i, int depth) {
    if (this->_verbose >= verbose::normal && depth == this->_current_max_depth) {
        char    tmp[256];
        std::sprintf(tmp, "  | %2d-%c : %11d pts\n", 19-(i/19), "ABCDEFGHJKLMNOPQRST"[i%19], score);
        this->_debug_string.append(tmp);
    }
}

void    AlphaBetaWithMemory::_debug_search(t_ret const& ret) {
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

static inline const int64_t player_evaluation(t_node const &node, uint8_t depth) {
    int64_t     score = 0;

    if (node.player_pairs_captured >= 5)
        return (500000 * depth);
    score += pair_capture_detector(node.player, node.opponent).set_count() * 50;//  5,000pts / pairs capture threatening
    score += node.player_pairs_captured * node.player_pairs_captured * 100; // pairs capture score
    return (score);
}

static inline const int64_t opponent_evaluation(t_node const &node, uint8_t depth) {
    int64_t     score = 0;

    if (node.opponent_pairs_captured >= 5)
        return (500000 * depth);
    score += pair_capture_detector(node.opponent, node.player).set_count() * 50;//  5,000pts / pairs capture threatening
    score += node.opponent_pairs_captured * node.opponent_pairs_captured * 100; // pairs capture score
    return (score);
}

int32_t        evaluation_function(t_node const &node, uint8_t depth) {
    int64_t     score = 0;
    /* we give more weight to the player whose turn is next */
    score += player_evaluation(node, depth) * (node.cid == 2 ? 3 : 1);
    score -= opponent_evaluation(node, depth) * (node.cid == 1 ? 3 : 1);
    return (score);
}


static inline const int64_t player_score(t_node const &node, uint8_t depth) {
    BitBoard    board;
    int64_t     score = 0;

    /* detect if current board has an unbreakable five */
    board = highlight_five_aligned(node.player ^ pair_capture_detector_highlight(node.opponent, node.player));
    if (!board.is_empty() && win_by_capture_detector(node.opponent, node.player, node.opponent_pairs_captured).is_empty())
        return (500000 * depth); // 50000000
    if (node.player_pairs_captured >= 5)
        return (500000 * depth); // 50,000,000pts / depth for win
    for (int i = 0; i < 11; ++i) {
        board = pattern_detector(node.player, node.opponent, BitBoard::patterns[i]);
        score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value : 0);
    }
    score += pair_capture_detector(node.player, node.opponent).set_count() * 50;//  5,000pts / pairs capture threatening
    score += node.player_pairs_captured * node.player_pairs_captured * 100;     // 10,000pts 40,000pts 90,000pts 160,000pts
    return (score);
}

static inline const int64_t opponent_score(t_node const &node, uint8_t depth) {
    BitBoard    board;
    int64_t     score = 0;

    /* detect if current board has an unbreakable five */
    board = highlight_five_aligned(node.opponent ^ pair_capture_detector_highlight(node.player, node.opponent));
    if (!board.is_empty() && win_by_capture_detector(node.player, node.opponent, node.player_pairs_captured).is_empty())
        return (500000 * depth);
    if (node.opponent_pairs_captured >= 5)
        return (500000 * depth);
    for (int i = 0; i < 11; ++i) {
        board = pattern_detector(node.opponent, node.player, BitBoard::patterns[i]);
        score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value : 0);
    }
    score += pair_capture_detector(node.opponent, node.player).set_count() * 50;//  5,000pts / pairs capture threatening
    score += node.opponent_pairs_captured * node.opponent_pairs_captured * 100; // 10,000pts 40,000pts 90,000pts 160,000pts
    return (score);
}

int32_t        score_function(t_node const &node, uint8_t depth) {
    int64_t     score = 0;
    /* we give more weight to the player whose turn is next */
    score += player_score(node, depth) * (node.cid == 2 ? 3 : 1);
    score -= opponent_score(node, depth) * (node.cid == 1 ? 3 : 1);
    return ((int32_t)range(score, (int64_t)-INF, (int64_t)INF));
}

t_ret           max(t_ret const& a, t_ret const& b) { return (a.score > b.score ? a : b); };
t_ret           min(t_ret const& a, t_ret const& b) { return (a.score < b.score ? a : b); };
int             max(int const& a, t_ret const& b)   { return (a > b.score ? a : b.score); };
int             min(int const& a, t_ret const& b)   { return (a < b.score ? a : b.score); };
int             max(int const& a, int const& b)     { return (a > b ? a : b); };
int             min(int const& a, int const& b)     { return (a < b ? a : b); };
int             range(int const& x, int const& min, int const& max) { return (x > max ? max : (x < min ? min : x)); };
int64_t         range(int64_t const& x, int64_t const& min, int64_t const& max) { return (x > max ? max : (x < min ? min : x)); };
