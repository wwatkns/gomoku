#include "AlphaBeta.hpp"

uint64_t    explored_nodes; // DEBUG

Eigen::Array2i  alphabeta_pruning(t_node *root, int32_t alpha, int32_t beta, int8_t depth) {
    BitBoard    moves = get_player_open_adjacent_positions(root->player, root->opponent) & ~root->player_forbidden;
    if (moves.set_count() == 0) {
        moves = get_player_open_adjacent_positions(root->opponent, root->player) & ~root->opponent_forbidden;
        if (moves.set_count() == 0)
            moves.write(9, 9);
    }
    explored_nodes = 0;
    // std::cout << moves << std::endl;

    int32_t     v;
    uint16_t    pos;
    t_node      tmp = *root;

    for (uint16_t i = 0; i < 361; ++i) {
        if (moves.check_bit(i)) {

            simulate_move(root, i);
            v = min(root, alpha, beta, depth-1);
            *root = tmp;
            // std::cout << v << std::endl;
            if (v > alpha) {
                alpha = v;
                pos = i;
            }
        }
    }
    std::cout << "nodes explored : " << explored_nodes << std::endl;
    return ((Eigen::Array2i){ pos / 19, pos % 19 }); // y, x, good
}

int32_t        max(t_node *node, int32_t alpha, int32_t beta, int8_t depth) {
    if (depth == 0 || check_end(node->opponent, node->player, node->opponent_pairs_captured, node->player_pairs_captured))
        return score_function(node, depth+1);

    BitBoard    moves = get_player_open_adjacent_positions(node->player, node->opponent) & ~node->player_forbidden;
    t_node      tmp = *node;

    for (uint16_t i = 0; i < 361; ++i) {
        if (moves.check_bit(i)) {
            simulate_move(node, i);
            alpha = max_val(alpha, min(node, alpha, beta, depth-1));
            *node = tmp;
            if (beta <= alpha)
                break;
        }
    }
    return (alpha);
}

int32_t        min(t_node *node, int32_t alpha, int32_t beta, int8_t depth) {
    if (depth == 0 || check_end(node->player, node->opponent, node->player_pairs_captured, node->opponent_pairs_captured))
        return score_function(node, depth+1);

    BitBoard    moves = get_player_open_adjacent_positions(node->opponent, node->player) & ~node->opponent_forbidden;
    t_node      tmp = *node;

    for (uint16_t i = 0; i < 361; ++i) {
        if (moves.check_bit(i)) {
            simulate_move(node, i);
            beta = min_val(beta, max(node, alpha, beta, depth-1));
            *node = tmp;
            if (beta <= alpha)
                break;
        }
    }
    return (beta);
}

void            simulate_move(t_node *node, uint16_t i) {
    BitBoard pairs;
    explored_nodes++;

    if (node->pid == 1) { /* simulate player move */
        pairs = pair_capture_detector(node->player, node->opponent);
        node->player.write(i);
        if ((pairs & node->player).is_empty() == false) {
            pairs = highlight_captured_stones(node->opponent, node->player, pairs);
            node->player_pairs_captured += pairs.set_count() / 2;
            node->opponent &= ~pairs;
        }
    } else { /* simulate opponent move */
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
    int32_t     score = 0;

    score += player_score(node, depth);
    score -= opponent_score(node, depth) * 2;
    return (score);
}

int32_t    player_score(t_node *node, uint8_t depth) {
    BitBoard    board;
    int32_t     score = 0;

    for (uint16_t i = 0; i < 10; i++) {
        board = current_pattern_detector(node->player, node->opponent, BitBoard::patterns[i]);
        score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value : 0);
        // Bonus x10 if patterns ends on opponent_forbidden, so he cannot counter
        // score += ((board & node->opponent_forbidden).is_empty() == false ? board.set_count() * BitBoard::patterns[i].value * 10 : 0);
    }
    if (detect_five_aligned(node->player)) score += 10000000 * depth; // 10,000,000pts for win by alignement
    if (node->player_pairs_captured == 5)  score +=  5000000 * depth; //  5,000,000pts for win by pairs captures
    score += node->player_pairs_captured * 50000;                     //     50,000pts/captures
    return (score);
}

int32_t    opponent_score(t_node *node, uint8_t depth) {
    BitBoard    board;
    int32_t     score = 0;

    for (uint16_t i = 0; i < 10; i++) {
        board = current_pattern_detector(node->opponent, node->player, BitBoard::patterns[i]);
        if (0 <= i && i <= 1)
            score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value * 2 : 0);
        else
            score += (board.is_empty() == false ? board.set_count() * BitBoard::patterns[i].value : 0);
        // Bonus x10 if patterns ends on opponent_forbidden, so he cannot counter
        // score += ((board & node->player_forbidden).is_empty() == false ? board.set_count() * BitBoard::patterns[i].value * 10 : 0);
    }
    if (detect_five_aligned(node->opponent)) score += 10000000 * depth; // 10,000,000pts for win by alignement
    if (node->opponent_pairs_captured == 5)  score +=  5000000 * depth; //  5,000,000pts for win by pairs captures
    score += node->opponent_pairs_captured * 50000;                     //     50,000pts/captures
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
