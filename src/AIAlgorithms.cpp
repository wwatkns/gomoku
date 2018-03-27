#include "AIAlgorithms.hpp"

/******************************************************* MINMAX *******************************************************/

MinMax::MinMax(void) {
}

MinMax::MinMax(MinMax const &src) {
    *this = src;
}

MinMax::~MinMax(void) {
}

MinMax          &MinMax::operator=(MinMax const&rhs) {
    return(*this);
}

t_best          MinMax::getmove(t_node node, int depth) {
    t_best      bestmove;

    bestmove = this->minmax(node, depth, 1);
    return (bestmove);
}

/* Simple minmax algorithm */
t_best          MinMax::minmax(t_node node, int depth, int player) {
    t_best      best;
    int         value;

    this->nbnode++; // DEBUG
    if (depth == 0 || check_end(node)) {
        this->nbleaf++; // DEBUG
        return ((t_best){ this->score_function(node, depth + 1), -INF });
    }
    else if (player) {
        best = { -INF, -INF };
        BitBoard moves = this->get_moves(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured,
                                         node.opponent_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->minmax(this->simulate_move(node, i), depth - 1, !player).score;
                best = value > best.score ? (t_best){ value, i } : best;
            }
        }
    }
    else {
        best = { INF, -INF };
        BitBoard moves = this->get_moves(node.opponent, node.player, node.opponent_forbidden,
                                         node.opponent_pairs_captured, node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->minmax(this->simulate_move(node, i), depth - 1, !player).score;
                best = value < best.score ? (t_best){ value, i } : best;
            }
        }
    }
    return (best);
}

/* This is the negamax version for posterity.

t_best          MinMax::negamax(t_node node, int depth, int color) {
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
            moves = this->get_moves(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured,
                                    node.opponent_pairs_captured);
        if (color == -1)
            moves = this->get_moves(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured,
                                    node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = -this->negamax(this->simulate_move(node, i), depth - 1, -color).score;
                best = value > best.score ? (t_best){ value, i } : best;
            }
        }
    }
    return (best);
}
*/

/***************************************************** ALPHABETA ******************************************************/

AlphaBeta::AlphaBeta(void) {
}

AlphaBeta::AlphaBeta(AlphaBeta const &src) {
    *this = src;
}

AlphaBeta::~AlphaBeta(void) {
}

AlphaBeta        &AlphaBeta::operator=(AlphaBeta const&rhs) {
    return(*this);
}

t_best          AlphaBeta::getmove(t_node node, int depth) {
    t_best      bestmove;

    bestmove = this->alphabeta(node, depth, -INF, INF, 1);
    return (bestmove);
}

/* Minmax with alpha-beta pruning */
t_best          AlphaBeta::alphabeta(t_node node, int depth, int alpha, int beta, int player) {
    t_best      best;
    int         value;

    this->nbnode++; // DEBUG
    if (depth == 0 || check_end(node)) {
        this->nbleaf++; // DEBUG
        return ((t_best){ this->score_function(node, depth + 1), -INF });
    }
    else if (player) {
        best = { -INF, -INF };
        BitBoard moves = this->get_moves(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured, 
                                         node.opponent_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->alphabeta(this->simulate_move(node, i), depth - 1, alpha, beta, !player).score;
                best = value > best.score ? (t_best){ value, i } : best;
                alpha = this->max(alpha, best.score);
                if (beta <= alpha) {
                    break;
                }
            }
        }
    }
    else {
        best = { INF, -INF };
        BitBoard moves = this->get_moves(node.opponent, node.player, node.opponent_forbidden,
                                         node.opponent_pairs_captured, node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->alphabeta(this->simulate_move(node, i), depth - 1, alpha, beta, !player).score;
                best = value < best.score ? (t_best){ value, i } : best;
                beta = this->min(beta, best.score);
                if (beta <= alpha) {
                    break;
                }
            }
        }
    }
    return (best);
}

/* This is the negamax version for posterity.

t_best          AlphaBeta::alphabetanegamax(t_node node, int depth, int alpha, int beta, int color) {
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
            moves = this->get_moves(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured,
                                    node.opponent_pairs_captured);
        if (color == -1)
            moves = this->get_moves(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured,
                                    node.player_pairs_captured);
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
*/

/***************************************************** NEGASCOUT ******************************************************/

NegaScout::NegaScout(void) {
}

NegaScout::NegaScout(NegaScout const &src) {
    *this = src;
}

NegaScout::~NegaScout(void) {
}

NegaScout        &NegaScout::operator=(NegaScout const&rhs) {
    return(*this);
}

t_best          NegaScout::getmove(t_node node, int depth) {
    t_best      bestmove;

    bestmove = this->negascout(node, depth, -INF, INF, 1, depth);
    return (bestmove);
}

t_best          NegaScout::negascout(t_node node, int depth, int alpha, int beta, int color, int max_depth) {
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
            moves = this->get_moves(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured,
                                    node.opponent_pairs_captured);
        if (color == -1)
            moves = this->get_moves(node.opponent, node.player, node.opponent_forbidden, node.opponent_pairs_captured,
                                    node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                if (depth == max_depth) {
                    value = -this->negascout(this->simulate_move(node, i), depth - 1, -beta, -alpha, -color, max_depth).score;
                }
                else {
                    value = -this->negascout(this->simulate_move(node, i), depth - 1, -alpha - 1, -beta, -color, max_depth).score;
                    if (alpha < value && value < beta) {
                        value = -this->negascout(this->simulate_move(node, i), depth - 1, -beta, -value, -color, max_depth).score;
                    }
                }
                alpha = this->max(alpha, value);
                best = value > best.score ? (t_best){ value, i } : best;
                if (alpha >= beta)
                    break;
            }
        }
    }
    return (best);
}

/******************************************************** MTDF ********************************************************/

MTDf::MTDf(void) {
}

MTDf::MTDf(MTDf const &src) {
    *this = src;
}

MTDf::~MTDf(void) {
}

MTDf            &MTDf::operator=(MTDf const&rhs) {
    return(*this);
}

t_best          MTDf::getmove(t_node node, int depth) {
    t_best      bestmove;

    bestmove = this->iterativedeepening(node, depth);
    return (bestmove);

}

/* Alphabeta with memory using transposition table */
t_best          MTDf::alphabetawithmemory(t_node node, int depth, int alpha, int beta, int player) {
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
        BitBoard moves = this->get_moves(node.player, node.opponent, node.player_forbidden, node.player_pairs_captured,
                                         node.opponent_pairs_captured);
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
        BitBoard moves = this->get_moves(node.opponent, node.player, node.opponent_forbidden,
                                         node.opponent_pairs_captured, node.player_pairs_captured);
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

t_best          MTDf::mtdf(t_node node, t_best f, int depth) {
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

t_best          MTDf::iterativedeepening(t_node node, int maxdepth) {
    // std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    t_best      g = { 0, 0 };

    for (int depth = 1; depth < maxdepth; (depth = depth + 2)) {
        g = this->mtdf(node, g, depth);
        // if (this->timesup(start)) {
        //     break;
        // }
    }
    this->_TT.clear();
    return (g);
}

/* This is the negamax version for posterity.

t_best          MTDf::negamaxwithmemory(t_node node, int depth, int alpha, int beta, int color) {
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
*/