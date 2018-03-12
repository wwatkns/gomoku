#include <stdio.h>
#include "NegaMax.hpp"

/**
 * NegaMax
 *
 * This is the implementation of NegaMax with AlphaBeta pruning and transposition table. It relies on the fact that the
 * score function should be the same for both players. In fact, it uses the fact that $ max(A, B) == -min(-A, -N) $.
 * This is a recursive implementation.
 */

Eigen::Array2i  negamaxplay(t_node *node, int8_t depth) {
    BitBoard    moves = get_player_open_adjacent_positions(node->opponent, node->player) & ~node->opponent_forbidden;
    if (moves.set_count() == 0) {
        moves = get_player_open_adjacent_positions(node->opponent, node->player) & ~node->opponent_forbidden;
        if (moves.set_count() == 0)
            moves.write(9, 9);
    }

    t_ret       ret = { 0, 0 };

    // int8_t      color       = 1;
    // t_node      tmp         = *node;
    // int32_t     v           = -INF;
    // int32_t     bestscore   = -INF;
    // int32_t     bestmove    = 0;
    // int         max;
    // int         alpha       = -INF;
    // int         beta        = INF;

    // std::cout << moves << std::endl;
    // for (int i = 0; i < 361; ++i) {
    //     if (moves.check_bit(i)) {
    //         simulate_move(node, i);
    //         v = -negamax_alphabeta(node, depth - 1, color, -INF, INF);
    //         std::cout << "score: " << v << std::endl;
    //         if (v > bestscore) {
    //             bestscore = v;
    //             bestmove  = i;
    //             if (bestscore > alpha) {
    //                 alpha = bestscore;
    //                 if (alpha >= beta) {
    //                     // std::cout << "depth: " << std::to_string(depth) << std::endl;                        
    //                     break ;
    //                 }
    //             }
    //         }
    //         *node = tmp;
    //     }
    // }

    // return ((Eigen::Array2i){ bestmove / 19, bestmove % 19 });

    ret = negamax_alphabeta(node, depth, -INF, INF, 1);
    return ((Eigen::Array2i){ ret.pos / 19, ret.pos % 19 });
}

t_ret         negamax_alphabeta(t_node *node, int8_t depth, int alpha, int beta, int color) {
    if (depth == 0 || (color == -1 && check_end(node->opponent, node->player, node->opponent_pairs_captured, node->player_pairs_captured))
                   || (color ==  1 && check_end(node->player, node->opponent, node->player_pairs_captured, node->opponent_pairs_captured))) {
        // int32_t score = negamax_score_function(node, depth, 1);
        // std::cout << "color: " << std::to_string(color) << " score: " << score << std::endl;
        return (negamax_score_function(node, depth, color));
    }
    t_node      tmp         = *node;
    int         v;
    int         bestscore   = -INF;
    BitBoard    moves;
    if (color == 1)
        moves = get_player_open_adjacent_positions(node->player, node->opponent) & ~node->player_forbidden;
    else
        moves = get_player_open_adjacent_positions(node->opponent, node->player) & ~node->opponent_forbidden;

    for (int i = 0; i < 361; ++i) {
        if (moves.check_bit(i)) {
            simulate_move(node, i);
            v = -negamax_alphabeta(node, depth - 1, -beta, -alpha, -color);
            if (v > bestscore) {
                bestscore = v;
                if (bestscore > alpha) {
                    alpha = bestscore;
                    if (alpha > beta)
                        return (bestscore);
                }
            }
            *node = tmp;
        }
    }
    
}

// int32_t         negamax_alphabeta(t_node *node, int8_t depth, int8_t color, int alpha, int beta) {
//     if (depth == 0 || (color == -1 && check_end(node->opponent, node->player, node->opponent_pairs_captured, node->player_pairs_captured))
//                    || (color ==  1 && check_end(node->player, node->opponent, node->player_pairs_captured, node->opponent_pairs_captured))) {
//         int32_t score = negamax_score_function(node, depth, 1);
//         // std::cout << "color: " << std::to_string(color) << " score: " << score << std::endl;
//         return (score);
//     }

//     // std::pair<int32_t, Eigen::Array2i>  node(0, {0, 0});
//     // Not sure...
//     BitBoard    moves;
//     if (color == 1)
//         moves = get_player_open_adjacent_positions(node->player, node->opponent) & ~node->player_forbidden;
//     else
//         moves = get_player_open_adjacent_positions(node->opponent, node->player) & ~node->opponent_forbidden;

//     t_node      tmp         = *node;
//     int         bestscore   = -INF;
//     int         v;

//     for (int i = 0; i < 361; ++i) {
//         if (moves.check_bit(i)) {
//             simulate_move(node, i);
//             v = -negamax_alphabeta(node, depth - 1, -color, -beta, -alpha);
//             // bestscore = max_val(v, bestscore);
//             if (v > bestscore) {
//                 bestscore = v;
//                 if (bestscore > alpha) {
//                     alpha = bestscore;
//                     if (alpha >= beta) {
//                         // std::cout << "depth: " << std::to_string(depth) << std::endl;
//                         return (bestscore);
//                     }
//                 }
//             }
//             *node = tmp;
//         }
//     }
//     // std::cout << "depth: " << std::to_string(depth) << std::endl;
//     return (bestscore);

// }

int32_t         negamax(t_node *node, int8_t depth, int8_t color) {
    if (depth == 0 || (color == -1 && check_end(node->opponent, node->player, node->opponent_pairs_captured, node->player_pairs_captured))
                   || (color ==  1 && check_end(node->player, node->opponent, node->player_pairs_captured, node->opponent_pairs_captured))) {
        int32_t score = color * negamax_score_function(node, depth, 1);
        // std::cout << "color: " << std::to_string(color) << " score: " << score << std::endl;
        return (score);
    }

    // std::pair<int32_t, Eigen::Array2i>  node(0, {0, 0});
    // Not sure...
    BitBoard    moves;
    if (color == 1)
        moves = get_player_open_adjacent_positions(node->player, node->opponent) & ~node->player_forbidden;
    else
        moves = get_player_open_adjacent_positions(node->opponent, node->player) & ~node->opponent_forbidden;

    t_node      tmp         = *node;
    int32_t     bestscore   = -INF;
    int32_t     v;

    for (int i = 0; i < 361; ++i) {
        if (moves.check_bit(i)) {
            simulate_move(node, i);
            v = -negamax(node, depth - 1, -color);
            bestscore = max_val(v, bestscore);
            *node = tmp;
        }
    }
    return (bestscore);

}

t_ret         negamax_score_function(t_node *node, uint8_t depth, int pos, int8_t color) {
    int32_t     score = 0;
    BitBoard    board;


    for (uint16_t i = 0; i < 10; i++) {
        if (color == -1)
            board = current_pattern_detector(node->player, node->opponent, BitBoard::patterns[i]);
        else
            board = current_pattern_detector(node->opponent, node->player, BitBoard::patterns[i]);
        score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value : 0);
        // Bonus x10 if patterns ends on opponent_forbidden, so he cannot counter
        score += ((board & node->opponent_forbidden).is_empty() == false ? board.set_count() * BitBoard::patterns[i].value * 10 : 0);
    }
    if (detect_five_aligned(node->player)) score += 10000000 * (depth + 1); // 10,000,000pts for win by alignement
    if (node->player_pairs_captured == 5)  score +=  5000000 * (depth + 1); //  5,000,000pts for win by pairs captures
    score += node->player_pairs_captured * 50000;                     //     50,000pts/captures
    return ({pos, score});
}
