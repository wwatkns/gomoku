#include "AlphaBeta.hpp"

uint64_t    explored_nodes = 0; // DEBUG

Eigen::Array2i  alphabeta_pruning(t_node *root, int8_t depth) {
    BitBoard    moves = root->player.dilated() & (~root->player & ~root->opponent);
    if (moves.set_count() == 0) {
        moves = root->opponent.dilated() & (~root->player & ~root->opponent);
        if (moves.set_count() == 0)
            moves.write(9, 9);
    }
    explored_nodes = 0;
    
    int32_t     max_v = -INF;
    int32_t     v;
    uint16_t    pos;
    t_node      tmp = *root;

    for (uint16_t i = 0; i < 361; i++) {
        if (moves.check_bit(i)) {

            simulate_move(root, i);
            v = min(root, -INF, INF, depth-1);
            *root = tmp;

            std::cout << v << std::endl;

            if (v > max_v) {
                pos = i;
                max_v = v;
            }
        }
    }
    std::cout << "nodes explored : " << explored_nodes << std::endl;
    return ((Eigen::Array2i){ pos / 19, pos % 19 }); // y, x, good
}

int32_t        max(t_node *node, int32_t alpha, int32_t beta, int8_t depth) {
    if (depth == 0 || check_end(node->opponent, node->player, node->opponent_pairs_captured, node->player_pairs_captured)) {
        return score_function(node, depth+1);
    }
    explored_nodes++;

    BitBoard    moves = get_player_open_adjacent_positions(node->player, node->opponent) & ~node->player_forbidden;
    t_node      tmp = *node;
    int32_t     v = -INF;

    for (uint16_t i = 0; i < 361; i++) {
        if (moves.check_bit(i)) {

            simulate_move(node, i);
            v = max_val(v, min(node, alpha, beta, depth-1));
            *node = tmp;

            alpha = max_val(alpha, v);
            if (beta <= alpha)
                break;
        }
    }
    return (v);
}

int32_t        min(t_node *node, int32_t alpha, int32_t beta, int8_t depth) {
    if (depth == 0 || check_end(node->player, node->opponent, node->player_pairs_captured, node->opponent_pairs_captured)) {
        return score_function(node, depth+1);
    }
    explored_nodes++;

    BitBoard    moves = get_player_open_adjacent_positions(node->opponent, node->player) & ~node->opponent_forbidden;
    t_node      tmp = *node;
    int32_t     v = INF;

    for (uint16_t i = 0; i < 361; i++) {
        if (moves.check_bit(i)) {

            simulate_move(node, i);
            v = min_val(v, max(node, alpha, beta, depth-1));
            *node = tmp;

            beta = min_val(beta, v);
            if (beta <= alpha)
                break;
        }
    }
    return (v);
}

void            simulate_move(t_node *node, uint16_t i) {
    BitBoard pairs;

    if (node->pid == 1) {
        /* simulate player move */
        pairs = pair_capture_detector(node->player, node->opponent);
        node->player.write(i);
        if ((pairs & node->player).is_empty() == false) {
            pairs = highlight_captured_stones(node->opponent, node->player, pairs);
            node->player_pairs_captured += pairs.set_count() / 2;
            node->opponent &= ~pairs;
        }
    } else {
        /* simulate opponent move */
        pairs = pair_capture_detector(node->opponent, node->player);
        node->opponent.write(i);
        if ((pairs & node->opponent).is_empty() == false) {
            pairs = highlight_captured_stones(node->player, node->opponent, pairs);
            node->opponent_pairs_captured += pairs.set_count() / 2;
            node->player &= ~pairs;
        }
    }
    node->player_forbidden   = forbidden_detector(node->player, node->opponent);
    node->opponent_forbidden = forbidden_detector(node->opponent, node->player);
    node->pid = ~node->pid & 0x3;
}

int32_t        score_function(t_node *node, uint8_t depth) {
    BitBoard    board;
    int32_t     score = 0;

    /* compute */
    for (uint16_t i = 0; i < 11; i++) {
        // if (node->pid == 1)
        //     board = current_pattern_detector(node->opponent, node->player, BitBoard::patterns[i]);
        // else
        //     board = current_pattern_detector(node->player, node->opponent, BitBoard::patterns[i]);
        //
        // score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value : 0);

        board = current_pattern_detector(node->player, node->opponent, BitBoard::patterns[i]);
        score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value : 0);

        // board = current_pattern_detector(node->opponent, node->player, BitBoard::patterns[i]);
        // score -= (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value : 0);
    }
    score = (detect_five_aligned(node->player) ? 528287 * depth : score);
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
bool check_end(BitBoard const& player, BitBoard const& opponent, uint8_t const& player_pairs_captured, uint8_t const& opponent_pairs_captured) {
    if (player_pairs_captured >= 5)
        return (true);
    if (detect_five_aligned(player) == true)
        return (true);
    if (((player | opponent) ^ BitBoard::full).is_empty() == true)
        return (true);
    return (false);
}

int32_t         min_val(int32_t const &a, int32_t const &b) {
    return (a < b ? a : b);
}

int32_t         max_val(int32_t const &a, int32_t const &b) {
    return (a > b ? a : b);
}
