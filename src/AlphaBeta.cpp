#include "AlphaBeta.hpp"

Eigen::Array2i  alphabeta_pruning(t_node root, uint8_t depth) {
    BitBoard    moves = root.p2.dilated() & ~root.p1 & ~root.p2;
    std::cout << moves.set_count() << std::endl;
    if (moves.set_count() == 0) {
        moves = root.p1.dilated() & ~root.p1 & ~root.p2;
    }

    std::cout << moves << std::endl;
    // BitBoard    moves = get_player_open_adjacent_positions(root.p1, root.p2) & ~root.p1_forbidden & (~root.p1 & ~root.p2);
    uint32_t    best_score = 0;
    uint32_t    score = 0;
    uint32_t    alpha;
    uint32_t    beta;
    int         best_pos;

    for (int i = 0; i < 361; i++) {
        if (moves[i] != 0) {
            root.p2.write(i);
            score = min(root, 0, 0xFFFFFFFF, depth);
            if (score > best_score) {
                best_pos = i;
                best_score = score;
            }
            root.p2.remove(i);
        }
    }
    // return ((Eigen::Array2i){0, 0});
    return ((Eigen::Array2i){best_pos % 19, best_pos / 19}); // x, y
}

uint32_t        max(t_node node, uint32_t alpha, uint32_t beta, uint8_t current_depth) {
    node.p1_forbidden = forbidden_detector(node.p1, node.p2);
    BitBoard                    moves = node.p1.dilated() & ~node.p1 & ~node.p2;
    // BitBoard                    moves = get_player_open_adjacent_positions(node.p1, node.p2) & ~node.p1_forbidden & (~node.p1 & ~node.p2);
    uint32_t                    score;
    uint32_t                    max;

    node.current_player_id = (node.current_player_id == 1 ? 2 : 1);
    uint8_t cpc = (node.current_player_id == 1 ? node.p1_pairs_captured : node.p2_pairs_captured);
    if (current_depth == 0 || check_end(node.p1, node.p2, node.p1_pairs_captured, node.p2_pairs_captured)) {
        node.p2_forbidden = forbidden_detector(node.p2, node.p1);
        return score_function(node);
    }
    max = 0;
    for (uint16_t i = 0; i < 361; i++) {
        if (moves[i] != 0) {

            node.p1.write(i);

            score = min(node, alpha, beta, current_depth-1);
            max = (score > max ? score : max);
            alpha = (alpha > score ? alpha : score);
            if (beta <= alpha)
                break;

            node.p1.remove(i);
        }
    }
    return (max);
}

uint32_t        min(t_node node, uint32_t alpha, uint32_t beta, uint8_t current_depth) {
    node.p2_forbidden = forbidden_detector(node.p2, node.p1);
    BitBoard                    moves = node.p2.dilated() & ~node.p1 & ~node.p2;
    // BitBoard                    moves = get_player_open_adjacent_positions(node.p2, node.p1) & ~node.p2_forbidden & (~node.p1 & ~node.p2);
    uint32_t                    score;
    uint32_t                    min;

    node.current_player_id = (node.current_player_id == 1 ? 2 : 1);
    uint8_t cpc = (node.current_player_id == 1 ? node.p1_pairs_captured : node.p2_pairs_captured);
    if (current_depth == 0 || check_end(node.p2, node.p1, node.p2_pairs_captured, node.p1_pairs_captured)) {
        node.p1_forbidden = forbidden_detector(node.p1, node.p2);
        return score_function(node);
    }
    min = 0xFFFFFFFF;
    for (uint16_t i = 0; i < 361; i++) {
        if (moves[i] != 0) {

            node.p2.write(i);

            score = max(node, alpha, beta, current_depth-1);
            min = (score < min ? score : min);
            beta = (beta < score ? beta : score);
            if (beta <= alpha)
                break;

            node.p2.remove(i);
        }
    }
    return (min);
}

uint32_t        score_function(t_node node) {
    std::array<BitBoard, 11>    boards;
    uint32_t                    score;

    for (uint16_t i = 0; i < 11; i++) {
        boards[i] = pattern_detector(node.p2, node.p1, BitBoard::patterns[i]);
        // boards[i] = pattern_detector(node.p1, node.p2, BitBoard::patterns[i]);
    }
    for (uint16_t j = 0; j < 361; j++) {
        for (uint16_t i = 0; i < 11; i++) {
            score += (boards[i][j] != 0) * BitBoard::patterns[i].value;
        }
    }
    return (score);
}

uint8_t check_end(BitBoard const& p1, BitBoard const& p2, uint8_t const& p1_pairs_captured, uint8_t const& p2_pairs_captured) {
    if (p1_pairs_captured >= 5)
        return (1);
    if (detect_five_aligned(p1)) {
        BitBoard    pairs = pair_capture_detector(p2, p1); // good
        /* p2 wins next turn by capturing a fifth pair even though p1 has 5 aligned */
        if (!pairs.is_empty() && p2_pairs_captured == 4)
            return (0);
        /* game continue by capturing stones forming the 5 alignment */
        if ( detect_five_aligned(highlight_five_aligned(p1) ^ highlight_captured_stones(p1, p2 ^ pairs, pairs)) == false )
            return (0);
        return (1);
    }
    if (((p1 | p2) ^ BitBoard::full).is_empty() == true)
        return (2);
    return (0);
}
