#include "AIPlayer.hpp"
// #include "Computer.hpp"

AIPlayer::AIPlayer(void) {
    // Stack some room for transposition tables
    this->_TT.reserve(409600);
}

AIPlayer::AIPlayer(AIPlayer const &src) {
    *this = src;
}

AIPlayer::~AIPlayer(void) {
}

AIPlayer        &AIPlayer::operator=(AIPlayer const&) {
    return(*this);
}

BitBoard        AIPlayer::get_moves(BitBoard const& player, BitBoard const& opponent, BitBoard const& player_forbidden,
                                    int player_pairs_captured, int opponent_pairs_captured) {
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

t_node          AIPlayer::simulate_move(t_node const &node, int i) { // this is the biggest performance hit
    t_node      child = node;
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

int32_t         AIPlayer::score_function(t_node const &node, uint8_t depth) {
    int32_t     score = 0;

    // if (node.pid == 2) {
        score += this->player_score(node, depth);
        score -= this->opponent_score(node, depth) * 2;
    // } else {
        // score += opponent_score(node, depth);
        // score -= player_score(node, depth) * 2;
    // }
    return (score);
}

int32_t         AIPlayer::player_score(t_node const &node, uint8_t depth) {
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

int32_t         AIPlayer::opponent_score(t_node const &node, uint8_t depth) {
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

bool            AIPlayer::check_end(t_node const& node) {
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
