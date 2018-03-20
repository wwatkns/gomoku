#include "mtdf.hpp"
#include "ZobristTable.hpp"

#include <stdio.h> // debug

// TODO faire deux TT 
//  - lower
//  - upper

int             g_counter = 0;
int             g_nodes_explored = 0;

bool            times_up(std::chrono::steady_clock::time_point start, uint32_t limit) { // THIS FUNCTION WORKS
    return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start).count() > limit);
}

t_ret           max(t_ret const& a, t_ret const& b) { return (a.score > b.score ? a : b); }
t_ret           min(t_ret const& a, t_ret const& b) { return (a.score < b.score ? a : b); }
int             max(int const& a, t_ret const& b)   { return (a > b.score ? a : b.score); }
int             min(int const& a, t_ret const& b)   { return (a < b.score ? a : b.score); }
int             max(int const& a, int const& b)     { return (a > b ? a : b); }
int             min(int const& a, int const& b)     { return (a < b ? a : b); }

BitBoard     moves_to_explore(BitBoard const& player, BitBoard const& opponent, BitBoard const& player_forbidden, int player_pairs_captured, int opponent_pairs_captured) {
    BitBoard    moves;
    BitBoard    tmp;
    bool        player_empty = player.is_empty();
    bool        opponent_empty = opponent.is_empty();

    if (!player_empty) {
        moves = get_winning_moves(player, opponent, player_pairs_captured);
        if (!moves.is_empty())
            return (moves);
    }
    if (!player_empty && !opponent_empty)
        moves |= pair_capture_detector(player, opponent);
    if (!opponent_empty)
        moves |= get_threat_moves(player, opponent, opponent_pairs_captured);

    if (!player_empty) { /* if player has stones, check the winning positions first */
        moves |= get_winning_moves(player, opponent, player_pairs_captured);
        if (moves.is_empty()) /* if no threats or winning moves, dilate around stones */
            moves |= get_moves_to_explore(player, opponent) & ~player_forbidden;
    }
    else { /* if player has no stones */
        if (!opponent_empty && moves.is_empty()) /* if opponent has stones and we found no threats */
            moves |= get_moves_to_explore(opponent, player);
        else
            moves.write(9, 9);
    }
    return (moves & ~player & ~opponent);
}

Eigen::Array2i  iterative_deepening(t_node *root, int8_t max_depth) {
    std::chrono::steady_clock::time_point   start       = std::chrono::steady_clock::now();
    t_ret                                   g           = { 0, 0 };

    g_counter = 0;
    g_nodes_explored = 0;

    for (int depth = 1; depth <= max_depth; depth++) {
        std::cout << "depth: " << depth << std::endl;
        g = mtdf(root, g, depth);
        // if (times_up(start, 500)) {
            // std::cout << "TIMES UP, reached depth : " << depth << std::endl;
            // break;
        // }
    }
    ZobristTable::upperbound_map.clear();
    ZobristTable::lowerbound_map.clear();
    return ((Eigen::Array2i){ g.p / 19, g.p % 19 });
}

t_ret   mtdf(t_node *root, t_ret g, int8_t depth) {
    t_ret   best        = g;
    int32_t bound[2]    = { -INF, INF };
    int32_t beta;

    while (bound[0] < bound[1]) {
        beta = g.score + (g.score == bound[0]);
        g = ft_alphaBetaWithMemory(*root, beta-1, beta, depth, 1);
        if (g.p != -INF) {
            best = g;
        }
        bound[(g.score < beta)] = g.score;
    }
    return (best);
}

/*
function AlphaBetaWithMemory(n : node_type; alpha , beta , d : integer) : integer;
    if retrieve(n) == OK then // Transposition table lookup
        if n.lowerbound >= beta then
            return n.lowerbound;
        if n.upperbound <= alpha then
            return n.upperbound;
        alpha := max(alpha, n.lowerbound);
        beta := min(beta, n.upperbound);
    if d == 0 then
        g := evaluate(n); // leaf node
    else if n == MAXNODE then
        g := -INFINITY;
        a := alpha; // save original alpha value
        c := firstchild(n);
        while (g < beta) and (c != NOCHILD) do
            g := max(g, AlphaBetaWithMemory(c, a, beta, d - 1));
            a := max(a, g);
            c := nextbrother(c);
    else // n is a MINNODE
        g := +INFINITY;
        b := beta; // save original beta value
        c := firstchild(n);
        while (g > alpha) and (c != NOCHILD) do
            g := min(g, AlphaBetaWithMemory(c, alpha, b, d - 1));
            b := min(b, g);
            c := nextbrother(c);
    // Traditional transposition table storing of bounds
    // Fail low result implies an upper bound
    if g <= alpha then
        n.upperbound := g; 
        store n.upperbound;
    // Found an accurate minimax value - will not occur if called with zero window
    if g >  alpha and g < beta then
        n.lowerbound := g;
        n.upperbound := g;
        store n.lowerbound, n.upperbound;
    // Fail high result implies a lower bound
    if g >= beta then
        n.lowerbound := g;
        store n.lowerbound;
    return g;
*/

t_ret   ft_alphaBetaWithMemory(t_node node, int alpha, int beta, int depth, int player) {
    int             g;
    int             lowerbound;
    int             upperbound;
    int             a; // alpha save
    int             b; // beta save
    t_ret           best;

    std::cout << "\tdepth: " << depth << std::endl;
    // if (depth < 5) {
        // if (ZobristTable::lowerbound_map.find({node.player, node.opponent}) != ZobristTable::lowerbound_map.end()) {
        if (ZobristTable::lowerbound_map.count({node.player, node.opponent}) > 0) {
            lowerbound = ZobristTable::lowerbound_map[{node.player, node.opponent}];        
            if (lowerbound >= beta) // Fail high
            {
                std::cout << "Fail high" << std::endl;
                return ((t_ret){ lowerbound, -INF });
            }
            alpha = max(alpha, lowerbound);
        }
        // if (ZobristTable::upperbound_map.find({node.player, node.opponent}) != ZobristTable::upperbound_map.end()) {
        if (ZobristTable::upperbound_map.count({node.player, node.opponent}) > 0) {
            upperbound = ZobristTable::upperbound_map[{node.player, node.opponent}];
            if (upperbound <= alpha) // Fail Low
            {
                std::cout << "Fail low" << std::endl;
                return ((t_ret){ upperbound, -INF });
            }
            beta = min(beta, upperbound);
        }
    // }
    // PBLM : cas où j'ai une lowerbound ou juste une upperbound...
    // if (alpha >= beta)
        // return ((t_ret){ bound, -INF });

    if (depth == 0 || check_end(node)) {
        return ((t_ret){ score_function(node, depth+1), -INF });
    }

    // MAX PLAYER
    else if (player) {
        best = { alpha, -INF };
        BitBoard    moves = moves_to_explore(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, node.opponent_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                g = ft_alphaBetaWithMemory(simulate_move(node, i), best.score, beta, depth-1, 0).score;
                if (g > best.score)
                    best = { g, i };
                if (g >= beta) {
                    break;
                }
            }
        }
    }
    // MIN PLAYER
    else {
        best = { beta, -INF };
        BitBoard    moves = moves_to_explore(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured, node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                g = ft_alphaBetaWithMemory(simulate_move(node, i), alpha, best.score, depth-1, 1).score;
                if (g < best.score)
                    best = { g, i };
                if (g <= alpha) {
                    break;
                }
            }
        }
    }

    TT_store(node, best.score, alpha, beta);
    return (best);
}

void            TT_store(t_node const &node, int32_t g, int32_t alpha, int32_t beta) {
    if (g <= alpha) {
        ZobristTable::upperbound_map[{node.player, node.opponent}] = g;
    }
    else if (g > alpha && g < beta) {
        ZobristTable::lowerbound_map[{node.player, node.opponent}] = g;
        ZobristTable::upperbound_map[{node.player, node.opponent}] = g;
    }
    else if (g >= beta) {
        ZobristTable::lowerbound_map[{node.player, node.opponent}] = g;
    }
}

/*
void            TT_store(t_node const &node, int32_t g, int32_t alpha, int32_t beta) {
    t_stored    entry;

    std::cout << "TT_store ---------------------------------------------------------" << std::endl;
    if (g <= alpha) {
        entry.upperbound = g;
    }
    // else if (g > alpha && g < beta) {
    //     entry.lowerbound = g;
    //     entry.upperbound = g;
    // }
    else if (g >= beta) { // (g >= beta)
        entry.lowerbound = g;
    }
    std::cout << "---\t entry.lowerbound: " << entry.lowerbound << std::endl;
    std::cout << "---\t entry.upperbound: " << entry.upperbound << std::endl;
    
    // entry.depth = depth;
    // entry.score = g;
    // entry.pos  = pos;
    ZobristTable::map[{node.player, node.opponent}] = entry;

    std::cout << "TT_store - END ---------------------------------------------------" << std::endl;
    // Get an iterator pointing to begining of map
    // ZobristTable::iterator it = myMap.begin();
    // Iterate over the map using iterator
    // while(it != myMap.end())
    // {
    //     std::cout << "--- TT_store" << std::endl;
    //     std::cout << "---\t lowerbound: " << it.lowerbound << std::endl;
    //     std::cout << "---\t upperbound: " << it.upperbound << std::endl;
    //     // std::cout<< it. << " :: "<< it->second <<std::endl;
    //     it++;
    // }
}
*/

t_ret   alphaBetaWithMemory(t_node node, int alpha, int beta, int depth) {
    return (alphaBetaMax(node, alpha, beta, depth));
    // return (alphaBetaMax(node, alpha, beta, depth, 0));
}

// t_ret   alphaBetaMin(t_node node, int alpha, int beta, int depth, int lastpos) {
t_ret   alphaBetaMin(t_node node, int alpha, int beta, int depth) {
    t_ret       ret;
    t_ret       best = { INF, 0 };
    t_stored    entry;

    if (ZobristTable::map.count({node.player, node.opponent})) {
        entry = ZobristTable::map[{node.player, node.opponent}];
        if (entry.lowerbound >= beta)
            return ((t_ret){ entry.lowerbound, -INF });
        else if (entry.upperbound <= alpha)
            return ((t_ret){ entry.upperbound, -INF });
        alpha = max(alpha, entry.lowerbound);
        beta  = max(beta,  entry.upperbound);
    }

    int         cp_beta = beta;

    if (depth == 0 || check_end(node))
        return ((t_ret){ score_function(node, depth+1), -INF });
        // return ((t_ret){ score_function(node, depth+1), lastpos });

    BitBoard    moves = moves_to_explore(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured, node.player_pairs_captured);
    for (int i = 0; i < 361; ++i) {
        if (moves.check_bit(i)) {
            // ret = alphaBetaMax(simulate_move(node, i), alpha, beta, depth-1, i);
            ret.score = alphaBetaMax(simulate_move(node, i), alpha, beta, depth-1).score;
            if (ret.score < best.score) {
                best = { ret.score, i };
                beta = min(beta, best.score);
                // if (alpha >= beta) /* beta cut-off */
                if (ret.score > beta) /* beta cut-off */
                    break;
            }
        }
    }
    // TT_store(node, best.score, alpha, cp_beta, depth, best.p);
    return (best);
}

// t_ret   alphaBetaMax(t_node node, int alpha, int beta, int depth, int lastpos) {
t_ret   alphaBetaMax(t_node node, int alpha, int beta, int depth) {
    t_ret       ret;
    t_ret       best = { -INF, 0 };
    t_stored    entry;

    if (ZobristTable::map.count({node.player, node.opponent})) {
        entry = ZobristTable::map[{node.player, node.opponent}];
        if (entry.lowerbound >= beta)
            return ((t_ret){ entry.lowerbound, -INF });
        else if (entry.upperbound <= alpha)
            return ((t_ret){ entry.upperbound, -INF });
        alpha = max(alpha, entry.lowerbound);
        beta  = max(beta,  entry.upperbound);
    }

    int         cp_alpha = alpha;

    if (depth == 0 || check_end(node))
        return ((t_ret){ score_function(node, depth+1), -INF });
        // return ((t_ret){ score_function(node, depth+1), lastpos });

    BitBoard    moves = moves_to_explore(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, node.opponent_pairs_captured);
    for (int i = 0; i < 361; ++i) {
        if (moves.check_bit(i)) {
            // ret = alphaBetaMin(simulate_move(node, i), alpha, beta, depth-1, i);
            ret.score = alphaBetaMin(simulate_move(node, i), alpha, beta, depth-1).score;
            if (ret.score > best.score) {
                best = { ret.score, i };
                alpha = max(alpha, best.score);
                // if (alpha >= beta) /* alpha cut-off */
                if (alpha > ret.score) /* alpha cut-off */
                    break;
            }
        }
    }
    // TT_store(node, best.score, cp_alpha, beta, depth, best.p);
    return (best);
}

int32_t         TT_lookup(t_node const &node, int32_t alpha, int32_t beta, int8_t depth) {
    t_stored    entry;

    if (ZobristTable::map.count({node.player, node.opponent})) {
        entry = ZobristTable::map[{node.player, node.opponent}];

        if (entry.lowerbound >= beta)
            return (entry.lowerbound);
        else if (entry.upperbound <= alpha)
            return (entry.upperbound);

        // if (entry.depth >= depth) {
        //     if (entry.flag == ZobristTable::flag::exact)
        //         return (entry.score);
        //     else if (entry.flag == ZobristTable::flag::alpha && entry.score <= alpha)
        //         return (alpha);
        //     else if (entry.flag == ZobristTable::flag::beta && entry.score >= beta)
        //         return (beta);
        // }
    }
    return (-INF);
}

bool check_end(t_node const& node) {
    if (node.pid == 2) {
        if (node.player_pairs_captured >= 5 || detect_five_aligned(node.player))
            return (true);
    } else {
        if (node.opponent_pairs_captured >= 5 || detect_five_aligned(node.opponent))
            return (true);
    }
    if (((node.player | node.opponent) ^ BitBoard::full).is_empty() == true)
        return (true);
    return (false);
}

// t_ret   alphaBetaWithMemory(t_node root, int32_t alpha, int32_t beta, int8_t depth) {
//     // int32_t lookup = TT_lookup(root, alpha, beta, depth);
//     // if (lookup != -INF)
//     //     return ((t_ret){lookup, 171});
//
//     BitBoard    moves = get_moves_to_explore(root.player, root.opponent) & ~root.player_forbidden;
//     if (moves.set_count() == 0) {
//         moves = get_moves_to_explore(root.opponent, root.player) & ~root.opponent_forbidden;
//         if (moves.set_count() == 0)
//             moves.write(9, 9);
//     }
//
//     uint16_t        pos;
//     int             g = -INF;
//     int             a = alpha;
//
//     for (int i = 0; i < 361 && g < beta; ++i) {
//         if (moves.check_bit(i)) {
//             // v = min(simulate_move(root, i), alpha, beta, depth-1);
//             g = min(simulate_move(root, i), a, beta, depth-1);
//             if (g > a) { // max_val
//                 a = g;
//                 pos = i;
//             }
//         }
//     }
//     TT_store(root, g, alpha, beta, depth);
//     return ((t_ret){ a, pos });
// }
//
// int32_t        min(t_node node, int32_t alpha, int32_t beta, int8_t depth) {
//     // int32_t lookup = TT_lookup(node, alpha, beta, depth);
//     // if (lookup != -INF)
//     //     return (lookup);
//     t_stored    entry;
//
//     if (ZobristTable::map.count({node.player, node.opponent})) {
//         entry = ZobristTable::map[{node.player, node.opponent}];
//         // if (entry.depth >= depth) {
//             if (entry.flag == ZobristTable::flag::exact)
//                 return (entry.score);
//             else if (entry.flag == ZobristTable::flag::lowerbound && entry.score >= beta)
//                 alpha = entry.score;
//             else if (entry.flag == ZobristTable::flag::upperbound && entry.score <= alpha)
//                 beta = entry.score;
//         // }
//     }
//
//     if (depth == 0 || check_end(node.player, node.opponent, node.player_pairs_captured, node.opponent_pairs_captured)) {
//         int32_t score = score_function(node, depth+1);
//         TT_store(node, score, alpha, beta, depth);
//         return (score);
//     }
//
//     BitBoard    moves = get_moves_to_explore(node.opponent, node.player) & ~node.opponent_forbidden;
//     int32_t     b = beta; /* backup original */
//     int         g = INF;
//
//     for (int i = 0; i < 361 && g > alpha; ++i) {
//         if (moves.check_bit(i)) {
//             g = min_val(g, max(simulate_move(node, i), alpha, b, depth-1));
//             b = min_val(b, g);
//             // beta = min_val(beta, max(simulate_move(node, i), alpha, beta, depth-1));
//             // if (beta <= alpha)
//                 // break;
//         }
//     }
//     TT_store(node, g, alpha, beta, depth);
//     return (beta);
// }
//
// int32_t        max(t_node node, int32_t alpha, int32_t beta, int8_t depth) {
//     // int32_t lookup = TT_lookup(node, alpha, beta, depth);
//     // if (lookup != -INF)
//     //     return (lookup);
//     t_stored    entry;
//
//     if (ZobristTable::map.count({node.player, node.opponent})) {
//         entry = ZobristTable::map[{node.player, node.opponent}];
//         // if (entry.depth >= depth) {
//             if (entry.flag == ZobristTable::flag::exact)
//                 return (entry.score);
//             else if (entry.flag == ZobristTable::flag::lowerbound && entry.score >= beta)
//                 alpha = entry.score;
//             else if (entry.flag == ZobristTable::flag::upperbound && entry.score <= alpha)
//                 beta = entry.score;
//         // }
//     }
//
//
//     if (depth == 0 || check_end(node.opponent, node.player, node.opponent_pairs_captured, node.player_pairs_captured)) {
//         int32_t score = score_function(node, depth+1);
//         TT_store(node, score, alpha, beta, depth);
//         return (score);
//     }
//
//     BitBoard    moves = get_moves_to_explore(node.player, node.opponent) & ~node.player_forbidden;
//     int32_t     a = alpha; /* backup original */
//     int         g = -INF;
//
//     for (int i = 0; i < 361 && g < beta; ++i) {
//         if (moves.check_bit(i)) {
//             g = min(simulate_move(node, i), a, beta, depth-1);
//             a = max_val(a, g);
//             // alpha = max_val(alpha, min(simulate_move(node, i), a, beta, depth-1));
//             // alpha = max_val(alpha, min(simulate_move(node, i), alpha, beta, depth-1));
//             // if (beta <= alpha)
//             //     break;
//         }
//     }
//     // TT_store(node, alpha, a, beta, depth);
//     TT_store(node, g, alpha, beta, depth);
//     return (alpha);
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

    // if (node.pid == 2) {
        score += player_score(node, depth);
        score -= opponent_score(node, depth) * 2;
    // } else {
        // score += opponent_score(node, depth);
        // score -= player_score(node, depth) * 2;
    // }
    return (score);
}

int32_t    player_score(t_node const &node, uint8_t depth) {
    BitBoard    board;
    int32_t     score = 0;

    if (detect_five_aligned(node.player))
        return (214748364 * depth);
    if (node.player_pairs_captured == 5)
        return (214748364 * depth);
    for (int i = 0; i < 10; ++i) {
        board = pattern_detector(node.player, node.opponent, BitBoard::patterns[i]);
        score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value : 0);
    }
    score += node.player_pairs_captured * node.opponent_pairs_captured * 50000;
    // if (detect_five_aligned(node.player)) score += 10000000 * depth; // 10,000,000pts for win by alignement
    // if (node.player_pairs_captured == 5)  score +=  5000000 * depth; //  5,000,000pts for win by pairs captures
    return (score);
}

int32_t    opponent_score(t_node const &node, uint8_t depth) {
    BitBoard    board;
    int32_t     score = 0;

    if (detect_five_aligned(node.opponent))
        return (214748364 * depth);
    if (node.opponent_pairs_captured == 5)
        return (214748364 * depth);
    for (int i = 0; i < 10; ++i) {
        board = pattern_detector(node.opponent, node.player, BitBoard::patterns[i]);
        // if (0 <= i && i <= 1)
            // score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value * 2 : 0);
        // else
            score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value : 0);
    }
    // if (detect_five_aligned(node.opponent)) score += 10000000 * depth; // 10,000,000pts for win by alignement
    // if (node.opponent_pairs_captured == 5)  score +=  5000000 * depth; //  5,000,000pts for win by pairs captures
    score += node.opponent_pairs_captured * node.opponent_pairs_captured * 50000;
    // 50,000 : 100,000 : 200,000 : 400,000 : 800,000
    return (score);
}


// Does not handle the case where we continue to play without having done the capture of the pairs that breaks the five !
// uint8_t check_end(BitBoard const& p1, BitBoard const& p2, uint8_t const& p1_pairs_captured, uint8_t const& p2_pairs_captured) {
//     if (p1_pairs_captured >= 5)
//         return (1);
//     if (detect_five_aligned(p1)) {
//         BitBoard    pairs = pair_capture_detector(p2, p1); // good
//         /* p2 wins next turn by capturing a fifth pair even though p1 has 5 aligned */
//         if (!pairs.is_empty() && p2_pairs_captured == 4)
//             return (0);
//         /* game continue by capturing stones forming the 5 alignment */
//         if ( detect_five_aligned(highlight_five_aligned(p1) ^ highlight_captured_stones(p1, p2 ^ pairs, pairs)) == false )
//             return (0);
//         return (1);
//     }
//     if (((p1 | p2) ^ BitBoard::full).is_empty() == true)
//         return (2);
//     return (0);
// }

/* old version but simpler so we use it for now */
// bool check_end(BitBoard const& player, BitBoard const& opponent, uint8_t const& player_pairs_captured, uint8_t const& opponent_pairs_captured) {
//     if (player_pairs_captured >= 5)
//         return (true);
//     if (detect_five_aligned(player) == true)
//         return (true);
//     if (((player | opponent) ^ BitBoard::full).is_empty() == true)
//         return (true);
//     return (false);
// }
//
// int32_t         min_val(int32_t const &a, int32_t const &b) {
//     return (a < b ? a : b);
// }
//
// int32_t         max_val(int32_t const &a, int32_t const &b) {
//     return (a > b ? a : b);
// }
//
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
