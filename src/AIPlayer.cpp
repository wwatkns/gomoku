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

AIPlayer        &AIPlayer::operator=(AIPlayer const&rhs) {
    return(*this);
}

t_best          AIPlayer::minmax(t_node node, int depth, int player) {
    /* Simple minmax algorithm */
    t_best      best;
    int         value;

    if (depth == 0 || check_end(node)) {
        return ((t_best){ score_function(node, depth + 1), -INF });
    }
    else if (player) {
        best = { -INF, -INF };
        BitBoard moves = this->get_moves(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, node.opponent_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->minmax(this->simulate_move(node, i), depth - 1, !player).score;
                best = value > best.score ? (t_best){ value, i } : best;
            }
        }
    }
    else {
        best = { INF, -INF };
        BitBoard moves = this->get_moves(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured, node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->minmax(this->simulate_move(node, i), depth - 1, !player).score;
                best = value < best.score ? (t_best){ value, i } : best;
            }
        }
    }
    return (best);
}

t_best          AIPlayer::negamax(t_node node, int depth, int color) {
    /* Negamax */
    t_best      best;
    int         value;
    BitBoard    moves;

    this->nbnode++; // DEBUG
    if (depth == 0 || check_end(node)) {
        this->nbleaf++; // DEBUG
        return ((t_best){ (color * this->score_function(node, depth + 1)), -INF });
    }
    else {
        best = { -INF, -INF };
        if (color == 1)
            moves = this->get_moves(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, node.opponent_pairs_captured);
        if (color == -1)
            moves = this->get_moves(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured, node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = -this->negamax(this->simulate_move(node, i), depth - 1, -color).score;
                best = value > best.score ? (t_best){ value, i } : best;
            }
        }
    }
    return (best);
}


t_best          AIPlayer::alphabeta(t_node node, int depth, int alpha, int beta, int player) {
    /* Minmax with alpha-beta pruning */
    t_best      best;
    int         value;

    if (depth == 0 || check_end(node)) {
        return ((t_best){ score_function(node, depth + 1), -INF });
    }
    else if (player) {
        best = { -INF, -INF };
        BitBoard moves = this->get_moves(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, node.opponent_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->alphabeta(this->simulate_move(node, i), depth - 1, alpha, beta, !player).score;
                best = value > best.score ? (t_best){ value, i } : best;
                alpha = this->max(alpha, best.score);
                if (beta <= alpha) {
                    std::cout << "Cut max!" << std::endl;
                    break;
                }
            }
        }
    }
    else {
        best = { INF, -INF };
        BitBoard moves = this->get_moves(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured, node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->alphabeta(this->simulate_move(node, i), depth - 1, alpha, beta, !player).score;
                best = value < best.score ? (t_best){ value, i } : best;
                beta = this->min(beta, best.score);
                if (beta <= alpha) {
                    std::cout << "Cut min!" << std::endl;                    
                    break;
                }
            }
        }
    }
    return (best);
}

t_best          AIPlayer::alphabetanegamax(t_node node, int depth, int alpha, int beta, int color) {
    /* Negamax */
    t_best      best;
    int         value;
    BitBoard    moves;    

    this->nbnode++; // DEBUG
    if (depth == 0 || check_end(node)) {
        this->nbleaf++; // DEBUG
        return ((t_best){ (color * this->score_function(node, depth + 1)), -INF });
    }
    else {
        best = { -INF, -INF };
        if (color == 1)
            moves = this->get_moves(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, node.opponent_pairs_captured);
        if (color == -1)
            moves = this->get_moves(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured, node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = -this->alphabetanegamax(this->simulate_move(node, i), depth - 1, -beta, -alpha, -color).score;
                best = value > best.score ? (t_best){ value, i } : best;
                alpha = this->max(alpha, value);
                if (alpha >= beta)
                    break;
            }
        }
    }
    return (best);
}

/*
t_best          AIPlayer::alphabetawithmemory(t_node node, int depth, int alpha, int beta, int player) {
    // Alphabeta with memory using transposition table
    t_stored    bounds;
    t_best      best;
    int         value;

    this->nbnode++; // DEBUG

    if (this->_TT.count({ node.player, node.opponent }) > 0) {
        if (this->_TT[{ node.player, node.opponent }].depth >= depth) {
            if (this->_TT[{ node.player, node.opponent }].lowerbound >= beta) {
            return ((t_best){ this->_TT[{ node.player, node.opponent }].lowerbound, -INF });
            }
            if (this->_TT[{ node.player, node.opponent }].upperbound <= alpha) {
            return ((t_best){ this->_TT[{ node.player, node.opponent }].upperbound, -INF });
            }
        alpha = this->max(alpha, this->_TT[{ node.player, node.opponent }].lowerbound);
            beta = this->min(beta, this->_TT[{ node.player, node.opponent }].upperbound);
        }
    }

    if (depth == 0 || check_end(node)) {
        this->nbleaf++; // DEBUG
        return ((t_best){ this->score_function(node, depth + 1), -INF });
    }


    else if (player) {
        best = { -INF, -INF };
        int a = alpha;
        BitBoard moves = this->get_moves(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, node.opponent_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->alphabetawithmemory(this->simulate_move(node, i), depth - 1, a, beta, !player).score;
                best = value > best.score ? (t_best){ value, i } : best;
                a = this->max(a, best.score);
                if (best.score >= beta) {
                    break;
                }
            }
        }
    }
    else {
        best = { INF, -INF };
        int b = beta;
        BitBoard moves = this->get_moves(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured, node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->alphabetawithmemory(this->simulate_move(node, i), depth - 1, alpha, b, !player).score;
                best = value < best.score ? (t_best){ value, i } : best;
                b = this->min(b, best.score);
                if (best.score <= alpha) {
                    break;
                }
            }
        }
    }

    bounds.depth = depth;
    if (best.score <= alpha) {
        bounds.upperbound = alpha;
    }
    if (best.score > alpha && best.score < beta) {
        bounds.upperbound = alpha;
        bounds.lowerbound = beta;
    }
    if (best.score >= beta) {
        bounds.lowerbound = beta;
    }
    this->_TT[{ node.player, node.opponent }];
    return (best);
}
*/

t_best          AIPlayer::alphabetawithmemory(t_node node, int depth, int alpha, int beta, int player) {
    // Alphabeta with memory using transposition table version 2.0
    t_stored    bounds;
    t_best      best;
    int         value;

    this->nbnode++; // DEBUG

    if (this->_TT.count({ node.player, node.opponent }) > 0 && this->_TT[{ node.player, node.opponent }].depth >= depth) {
        if (this->_TT[{ node.player, node.opponent }].flag == 1) { // Exact
            return (this->_TT[{ node.player, node.opponent }].value);
        }
        else if (this->_TT[{ node.player, node.opponent }].flag == 2) { // Lowerbound
            alpha = this->max(alpha, this->_TT[{ node.player, node.opponent }].value);
        }
        else if (this->_TT[{ node.player, node.opponent }].flag == 3) { // Upperbound
            beta = this->min(beta, this->_TT[{ node.player, node.opponent }].value);
        }
        if (alpha >= beta) {
            return (this->_TT[{ node.player, node.opponent }].value);
        }
    }

    if (depth == 0 || check_end(node)) {
        this->nbleaf++; // DEBUG
        return ((t_best){ this->score_function(node, depth + 1), -INF });
    }


    else if (player) {
        best = { -INF, -INF };
        int a = alpha;
        BitBoard moves = this->get_moves(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, node.opponent_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->alphabetawithmemory(this->simulate_move(node, i), depth - 1, a, beta, !player).score;
                best = value > best.score ? (t_best){ value, i } : best;
                a = this->max(a, best.score);
                if (best.score >= beta) {
                    break;
                }
            }
        }
    }
    else {
        best = { INF, -INF };
        int b = beta;
        BitBoard moves = this->get_moves(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured, node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->alphabetawithmemory(this->simulate_move(node, i), depth - 1, alpha, b, !player).score;
                best = value < best.score ? (t_best){ value, i } : best;
                b = this->min(b, best.score);
                if (best.score <= alpha) {
                    break;
                }
            }
        }
    }

    bounds.value = best;
    bounds.depth = depth;
    if (best.score <= alpha) {
        bounds.flag = 3; // Upperbound
    }
    else if (best.score >= beta) {
        bounds.flag = 2; // Lowerbound
    }
    else {
        bounds.flag = 1; // Exact
    }
    this->_TT[{ node.player, node.opponent }] = bounds;
    return (best);
}

t_best          AIPlayer::negamaxwithmemory(t_node node, int depth, int alpha, int beta, int color) {
    /* Negamax */
    BitBoard    moves;
    t_stored    bounds;    
    t_best      best;
    int         value;
    int         a = alpha;

    this->nbnode++; // DEBUG
    if (this->_TT.count({ node.player, node.opponent }) > 0 && this->_TT[{ node.player, node.opponent }].depth >= depth) {
        if (this->_TT[{ node.player, node.opponent }].flag == 1) { // Exact
            return (this->_TT[{ node.player, node.opponent }].value);
        }
        else if (this->_TT[{ node.player, node.opponent }].flag == 2) { // Lowerbound
            alpha = this->max(alpha, this->_TT[{ node.player, node.opponent }].value);
        }
        else if (this->_TT[{ node.player, node.opponent }].flag == 3) { // Upperbound
            beta = this->min(beta, this->_TT[{ node.player, node.opponent }].value);
        }
        if (alpha >= beta) {
            return (this->_TT[{ node.player, node.opponent }].value);
        }
    }

    if (depth == 0 || check_end(node)) {
        this->nbleaf++; // DEBUG
        return ((t_best){ (color * this->score_function(node, depth + 1)), -INF });
    }
    else {
        best = { -INF, -INF };
        if (color == 1)
            moves = this->get_moves(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, node.opponent_pairs_captured);
        if (color == -1)
            moves = this->get_moves(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured, node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = -this->negamaxwithmemory(this->simulate_move(node, i), depth - 1, -beta, -alpha, -color).score;
                best = value > best.score ? (t_best){ value, i } : best;
                alpha = this->max(alpha, value);
                if (alpha >= beta)
                    break;
            }
        }
    }

    bounds.value = best;
    bounds.depth = depth;
    if (best.score <= a) {
        bounds.flag = 3; // Upperbound
    }
    else if (best.score >= beta) {
        bounds.flag = 2; // Lowerbound
    }
    else {
        bounds.flag = 1; // Exact
    }
    this->_TT[{ node.player, node.opponent }] = bounds;
    return (best);
}

t_best          AIPlayer::mtdf(t_node node, t_best f, int depth) {
    t_best      g = f;
    int         bounds[2] = { -INF, INF };
    int         beta;

    while (bounds[0] < bounds[1]) {
        beta = g.score + (g.score == bounds[0]);
        g = this->alphabetawithmemory(node, depth, beta - 1, beta, 1);
        bounds[g.score < beta] = g.score;
    }
    return (g);
}

t_best          AIPlayer::iterativedeepening(t_node node, int maxdepth) {
    t_best      g = { 0, 0 };

    for (int depth = 1; depth < maxdepth; (depth = depth + 2)) {
        g = this->mtdf(node, g, depth);
    }
    this->_TT.clear();
    return (g);
}

BitBoard        AIPlayer::get_moves(BitBoard const& player, BitBoard const& opponent, BitBoard const& player_forbidden, int player_pairs_captured, int opponent_pairs_captured) {
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
