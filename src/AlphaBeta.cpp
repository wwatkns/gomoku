#include "AlphaBeta.hpp"

uint64_t    explored_nodes = 0;

/*  depth of 2 does MAX, MIN, MAX 
*/

Eigen::Array2i  alphabeta_pruning(t_node *root, int8_t depth) {
    BitBoard    moves = root->player.dilated() & (~root->player & ~root->opponent);
    if (moves.set_count() == 0) {
        moves = root->opponent.dilated() & (~root->player & ~root->opponent);
        if (moves.set_count() == 0)
            moves.write(9, 9);
    }

    explored_nodes = 0; // debug
    // BitBoard    moves = get_player_open_adjacent_positions(root.p1, root->opponent) & ~root.p1_forbidden & (~root.p1 & ~root->opponent);
    int32_t     max = -INF;
    int32_t     f;
    uint16_t    pos;
    t_node      tmp = *root;

    for (uint16_t i = 0; i < 361; i++) {
        if (moves.check_bit(i)) {

            simulate_move(root, i);
            f = min(root, -INF, INF, depth-1);
            *root = tmp;

            if (f > max) {
                pos = i;
                max = f;
            }
        }
    }
    std::cout << "nodes explored : " << explored_nodes << std::endl;
    return ((Eigen::Array2i){ pos / 19, pos % 19 }); // y, x, good
}

int32_t        max(t_node *node, int32_t alpha, int32_t beta, int8_t depth) {
    // BitBoard    moves = get_player_open_adjacent_positions(node.player, node.opponent);
    BitBoard    moves = node->player.dilated() & (~node->player & ~node->opponent);
    t_node      tmp = *node;
    int32_t     v = -INF;

    explored_nodes++;

    if (depth <= 0 || check_end(node->player, node->opponent, node->player_pairs_captured, node->opponent_pairs_captured)) {
        return score_function(node);
    }
    for (uint16_t i = 0; i < 361; i++) {
        if (moves.check_bit(i)) {

            simulate_move(node, i);
            v = std::max(v, min(node, alpha, beta, depth-1));
            *node = tmp;

            alpha = std::max(alpha, v);
            if (beta <= alpha)
                break;
        }
    }
    return (v);
}

int32_t        min(t_node *node, int32_t alpha, int32_t beta, int8_t depth) {
    // BitBoard    moves = get_player_open_adjacent_positions(node->opponent, node->player);
    BitBoard    moves = node->opponent.dilated() & (~node->player & ~node->opponent);
    t_node      tmp = *node;
    int32_t     v = INF;

    explored_nodes++;

    if (depth <= 0 || check_end(node->opponent, node->player, node->opponent_pairs_captured, node->player_pairs_captured)) {
        return score_function(node);
    }
    for (uint16_t i = 0; i < 361; i++) {
        if (moves.check_bit(i)) {

            simulate_move(node, i);
            v = std::min(v, max(node, alpha, beta, depth-1));
            *node = tmp;

            beta = std::min(beta, v);
            if (beta <= alpha)
                break;
        }
    }
    return (v);
}

void            simulate_move(t_node *node, uint16_t i) {
    if (node->pid == 2) { // pid 1 or 2
        /* simulate player move */
        BitBoard pairs = pair_capture_detector(node->player, node->opponent);
        node->player.write(i);
        if ((pairs & node->player).is_empty() == false) {
            pairs = highlight_captured_stones(node->opponent, node->player, pairs);
            node->player_pairs_captured += pairs.set_count() / 2;
            node->opponent &= ~pairs;
        }
        node->player_forbidden = forbidden_detector(node->player, node->opponent);
        node->opponent_forbidden = forbidden_detector(node->opponent, node->player);

    } else {
        /* simulate opponent move */
        BitBoard pairs = pair_capture_detector(node->opponent, node->player);
        node->opponent.write(i);
        if ((pairs & node->opponent).is_empty() == false) {
            pairs = highlight_captured_stones(node->player, node->opponent, pairs);
            node->opponent_pairs_captured += pairs.set_count() / 2;
            node->player &= ~pairs;
        }
        node->player_forbidden = forbidden_detector(node->opponent, node->player);
        node->opponent_forbidden = forbidden_detector(node->player, node->opponent);
    }
    node->pid = ~node->pid & 0x3;
}

/*
    +---------+
    | . . . . | 0
    | . o . . | 1
    | . . o o | 2
    | . . . . | 3
    +---------+
      0 1 2 3

    this function should return :
    5, 10, 11
*/

int32_t        score_function(t_node *node) {
    BitBoard    board;
    int32_t     score = 0;

    /* compute */
    for (uint16_t i = 0; i < 11; i++) {
        if (node->pid == 2)
            board = pattern_detector(node->player, node->opponent, BitBoard::patterns[i]);
        else
            board = pattern_detector(node->opponent, node->player, BitBoard::patterns[i]);

        for (uint16_t j = 0; j < 361; j++) {
            score += (board.check_bit(j) ? BitBoard::patterns[i].value : 0);
        }
    }
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
uint8_t check_end(BitBoard const& player, BitBoard const& opponent, uint8_t const& player_pairs_captured, uint8_t const& opponent_pairs_captured) {
    if (player_pairs_captured >= 5)
        return (1);
    if (detect_five_aligned(player))
        return (1);
    if (((player | opponent) ^ BitBoard::full).is_empty() == true)
        return (1);
    return (0);
}
