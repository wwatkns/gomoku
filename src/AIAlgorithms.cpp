#include "AIAlgorithms.hpp"

/******************************************************* MINMAX *******************************************************/

MinMax::MinMax(int depth, uint8_t verbose) : AIPlayer(depth, verbose) {
}

MinMax::MinMax(MinMax const &src) : AIPlayer(src.get_depth(), src.get_verbose()) {
    *this = src;
}

MinMax::~MinMax(void) {
}

MinMax      &MinMax::operator=(MinMax const&) {
    return(*this);
}

t_ret const MinMax::operator()(t_node root) {
    return (this->minmax(root, this->_depth, 1));
}

/* Simple minmax algorithm */
t_ret       MinMax::minmax(t_node node, int depth, int player) {
    t_ret   best;
    int     value;

    this->nbnode++; // DEBUG
    if (depth == 0 || this->checkEnd(node)) {
        this->nbleaf++; // DEBUG
        return ((t_ret){ this->score_function(node, depth + 1), -INF });
    }
    else if (player) {
        best = { -INF, -INF };
        BitBoard moves = get_moves(node.player, node.opponent, forbidden_detector(node.player, node.opponent), node.player_pairs_captured,
                                         node.opponent_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->minmax(this->create_child(node, i), depth - 1, !player).score;
                best = value > best.score ? (t_ret){ value, i } : best;
            }
        }
    }
    else {
        best = { INF, -INF };
        BitBoard moves = get_moves(node.opponent, node.player, forbidden_detector(node.opponent, node.player),
                                         node.opponent_pairs_captured, node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->minmax(this->create_child(node, i), depth - 1, !player).score;
                best = value < best.score ? (t_ret){ value, i } : best;
            }
        }
    }
    return (best);
}

/***************************************************** ALPHABETA ******************************************************/

AlphaBeta::AlphaBeta(int depth, uint8_t verbose) : AIPlayer(depth, verbose) {
}

AlphaBeta::AlphaBeta(AlphaBeta const &src) : AIPlayer(src.get_depth(), src.get_verbose()) {
    *this = src;
}

AlphaBeta::~AlphaBeta(void) {
}

AlphaBeta   &AlphaBeta::operator=(AlphaBeta const&) {
    return(*this);
}

t_ret const AlphaBeta::operator()(t_node root) {
    return (this->alphabeta(root, this->_depth, -INF, INF, 1));
}

/* Minmax with alpha-beta pruning */
t_ret       AlphaBeta::alphabeta(t_node node, int depth, int alpha, int beta, int player) {
    t_ret   best;
    int     value;

    this->nbnode++; // DEBUG
    if (depth == 0 || this->checkEnd(node)) {
        this->nbleaf++; // DEBUG
        return ((t_ret){ this->score_function(node, depth + 1), -INF });
    }
    else if (player) {
        best = { -INF, -INF };
        BitBoard moves = get_moves(node.player, node.opponent, forbidden_detector(node.player, node.opponent), node.player_pairs_captured,
                                         node.opponent_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->alphabeta(this->create_child(node, i), depth - 1, alpha, beta, !player).score;
                best = value > best.score ? (t_ret){ value, i } : best;
                alpha = this->max(alpha, best.score);
                if (beta <= alpha) {
                    break;
                }
            }
        }
    }
    else {
        best = { INF, -INF };
        BitBoard moves = get_moves(node.opponent, node.player, forbidden_detector(node.opponent, node.player),
                                         node.opponent_pairs_captured, node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->alphabeta(this->create_child(node, i), depth - 1, alpha, beta, !player).score;
                best = value < best.score ? (t_ret){ value, i } : best;
                beta = this->min(beta, best.score);
                if (beta <= alpha) {
                    break;
                }
            }
        }
    }
    return (best);
}

/***************************************************** NEGASCOUT ******************************************************/

NegaScout::NegaScout(int depth, uint8_t verbose) : AIPlayer(depth, verbose) {
}

NegaScout::NegaScout(NegaScout const &src) : AIPlayer(src.get_depth(), src.get_verbose()) {
    *this = src;
}

NegaScout::~NegaScout(void) {
}

NegaScout   &NegaScout::operator=(NegaScout const&) {
    return(*this);
}

t_ret const NegaScout::operator()(t_node root) {
    return (this->negascout(root, this->_depth, -INF, INF, 1, this->_depth));
}

/* TODO: NegaScout relies on move ordering. Thus, this is NOT A WORKING NEGASCOUT! */
t_ret       NegaScout::negascout(t_node node, int depth, int alpha, int beta, int color, int max_depth) {
    t_ret       best;
    int         value;
    BitBoard    moves;

    this->nbnode++; // DEBUG
    if (depth == 0 || this->checkEnd(node)) {
        this->nbleaf++; // DEBUG
        return ((t_ret){ (color * this->score_function(node, depth + 1)), -INF });
    }
    else {
        best = { alpha, -INF };
        if (color == 1)
            moves = get_moves(node.player, node.opponent, forbidden_detector(node.player, node.opponent), node.player_pairs_captured,
                                    node.opponent_pairs_captured);
        if (color == -1)
            moves = get_moves(node.opponent, node.player, forbidden_detector(node.opponent, node.player), node.opponent_pairs_captured,
                                    node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                if (depth == max_depth) {
                    value = -this->negascout(this->create_child(node, i), depth - 1, -beta, -alpha, -color, max_depth).score;
                }
                else {
                    value = -this->negascout(this->create_child(node, i), depth - 1, -alpha - 1, -beta, -color, max_depth).score;
                    if (alpha < value && value < beta) {
                        value = -this->negascout(this->create_child(node, i), depth - 1, -beta, -value, -color, max_depth).score;
                    }
                }
                alpha = this->max(alpha, value);
                best = value > best.score ? (t_ret){ value, i } : best;
                if (alpha >= beta)
                    break;
            }
        }
    }
    return (best);
}

/******************************************************** MTDF ********************************************************/

MTDf::MTDf(int depth, uint8_t verbose) : AIPlayer(depth, verbose) {
    this->_TT.reserve(409600);
}

MTDf::MTDf(MTDf const &src) : AIPlayer(src.get_depth(), src.get_verbose()) {
    *this = src;
}

MTDf::~MTDf(void) {
}

MTDf        &MTDf::operator=(MTDf const&) {
    return(*this);
}

t_ret const MTDf::operator()(t_node root) {
    return (this->iterativedeepening(root, this->_depth));
}

/* Alphabeta with memory using transposition table */
t_ret       MTDf::alphabetawithmemory(t_node node, int depth, int alpha, int beta, int player) {
    t_stored    bounds;
    t_ret       best;
    int         value;

    this->nbnode++; // DEBUG
    if (this->timesup()) {
        return ((t_ret){ -INF, 0 });
    }
    if (this->_TT.count({ node.player, node.opponent }) > 0 && this->_TT[{ node.player, node.opponent }].depth >= depth) {
        t_stored stored = this->_TT[{ node.player, node.opponent }];
        if (stored.flag == ZobristTable::flag::exact) {
            return ((t_ret){ stored.score, stored.move });
        }
        else if (stored.flag == ZobristTable::flag::lowerbound) {
            alpha = this->max(alpha, (t_ret){ stored.score, stored.move });
        }
        else if (stored.flag == ZobristTable::flag::upperbound) {
            beta = this->min(beta, (t_ret){ stored.score, stored.move });
        }
        if (alpha >= beta) {
            return ((t_ret){ stored.score, stored.move });
        }
    }

    if (depth == 0 || this->checkEnd(node)) {
        this->nbleaf++; // DEBUG
        return ((t_ret){ this->score_function(node, depth + 1), -INF });
    }


    else if (player) {
        best = { -INF, -INF };
        int a = alpha;
        BitBoard moves = get_moves(node.player, node.opponent, forbidden_detector(node.player, node.opponent), node.player_pairs_captured,
                                         node.opponent_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->alphabetawithmemory(this->create_child(node, i), depth - 1, a, beta, !player).score;
                best = value > best.score ? (t_ret){ value, i } : best;
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
        BitBoard moves = get_moves(node.opponent, node.player, forbidden_detector(node.opponent, node.player),
                                         node.opponent_pairs_captured, node.player_pairs_captured);
        for (int i = 0; i < 361; ++i) {
            if (moves.check_bit(i)) {
                value = this->alphabetawithmemory(this->create_child(node, i), depth - 1, alpha, b, !player).score;
                best = value < best.score ? (t_ret){ value, i } : best;
                b = this->min(b, best.score);
                if (best.score <= alpha) {
                    break;
                }
            }
        }
    }

    bounds.score = best.score;
    bounds.move = best.p;
    bounds.depth = depth;
    if (best.score <= alpha)
        bounds.flag = ZobristTable::flag::upperbound;
    else if (best.score >= beta)
        bounds.flag = ZobristTable::flag::lowerbound;
    else
        bounds.flag = ZobristTable::flag::exact;
    this->_TT[{ node.player, node.opponent }] = bounds;
    return (best);
}

t_ret          MTDf::mtdf(t_node node, t_ret f, int depth) {
    t_ret   g = f;
    int     bounds[2] = { -INF, INF };
    int     beta;

    while (bounds[0] < bounds[1]) {
        beta = g.score + (g.score == bounds[0]);
        g = this->alphabetawithmemory(node, depth, beta - 1, beta, 1);
        bounds[g.score < beta] = g.score;
    }
    return (g);
}

t_ret          MTDf::iterativedeepening(t_node node, int maxdepth) {
    t_ret   g = { 0, 0 };
    t_ret   save;
    this->start = std::chrono::steady_clock::now();

    for (int depth = 1; depth < maxdepth; (depth = depth + 2)) {
        g = this->mtdf(node, g, depth);
        if (this->timesup()) {
            break;
        }
        save = g;
    }
    this->_TT.clear();
    return (save);
}

bool            MTDf::timesup(void) {
    if ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->start)).count() >= 499) {
        return (true);
    }
    return (false);
}

/************************************************** AlphaBetaCustom ***************************************************/

AlphaBetaCustom::AlphaBetaCustom(int depth, uint8_t verbose, int time_limit) :  AIPlayer(depth, verbose), _current_max_depth(0), _search_limit_ms(time_limit) {
    this->search_stopped = false;
}

AlphaBetaCustom::AlphaBetaCustom(AlphaBetaCustom const &src) : AIPlayer(src.get_depth(), src.get_verbose()) {
    *this = src;
}

AlphaBetaCustom::~AlphaBetaCustom(void) {
}

AlphaBetaCustom &AlphaBetaCustom::operator=(AlphaBetaCustom const &rhs) {
    (void)rhs;
    return (*this);
}

t_ret const     AlphaBetaCustom::operator()(t_node root) { /* iterative deepening */
    t_ret       ret = { 0, 0 };
    t_ret       current;

    this->search_stopped = false;
    this->_search_start = std::chrono::steady_clock::now();
    this->_root_moves.clear();

    for (this->_current_max_depth = 1; this->_current_max_depth <= this->_depth; this->_current_max_depth += 2) {
        current = _root_max(root, -INF, INF, this->_current_max_depth);
        // _debug_search(current);
        if (this->search_stopped)
            break;
        ret = current;
    }
    return (ret);
}

t_ret       AlphaBetaCustom::_min(t_node node, int alpha, int beta, int depth) {
    /* do we exceed our maximum allowed search time */
    if (this->_times_up())
        return ((t_ret){-INF, 0 });
    /* is the node a leaf or the game is won */
    if (depth == 0 || this->checkEnd(node))
        return ((t_ret){ this->score_function(node, depth+1), 0 });

    t_ret               current;
    t_ret               best = { INF, 0 };
    std::vector<t_move> moves = this->move_generation(node, depth);

    for (std::vector<t_move>::const_iterator move = moves.begin(); move != moves.end(); ++move) {
        current = this->_max(move->node, alpha, beta, depth-1);
        if (current < best) {
            best = { current.score, move->p };
            beta = this->min(beta, best.score);
            if (alpha >= beta) /* alpha cut-off */
                return (best);
        }
    }
    return (best);
}

t_ret       AlphaBetaCustom::_max(t_node node, int alpha, int beta, int depth) {
    /* do we exceed our maximum allowed search time */
    if (this->_times_up())
        return ((t_ret){ INF, 0 });
    /* is the node a leaf or the game is won */
    if (depth == 0 || this->checkEnd(node))
        return ((t_ret){ this->score_function(node, depth+1), 0 });

    t_ret               current;
    t_ret               best = {-INF, 0 };
    std::vector<t_move> moves = this->move_generation(node, depth);

    for (std::vector<t_move>::const_iterator move = moves.begin(); move != moves.end(); ++move) {
        current = this->_min(move->node, alpha, beta, depth-1);
        // _debug_append_explored(current.score, move->p, depth);
        if (current > best) {
            best = { current.score, move->p };
            alpha = this->max(alpha, best.score);
            if (alpha >= beta) /* beta cut-off */
                return (best);
        }
    }
    return (best);
}

t_ret       AlphaBetaCustom::_root_max(t_node node, int alpha, int beta, int depth) {
    t_ret       current;
    t_ret       best = {-INF, 0 };

    /* if we're at the top of our iterative deepening function */
    if (this->_current_max_depth == 1)
        this->_root_moves = this->move_generation(node, depth);

    /* otherwise the estimation at the previous iterative deepening loop will be used */
    for (std::vector<t_move>::iterator move = this->_root_moves.begin(); move != this->_root_moves.end(); ++move) {
        current = this->_min(move->node, alpha, beta, depth-1);
        move->eval = current.score;
        // _debug_append_explored(current.score, move->p, depth);
        if (current > best) {
            best = { current.score, move->p };
            alpha = this->max(alpha, best.score);
        }
    }
    std::sort(this->_root_moves.begin(), this->_root_moves.end(), sort_descending);
    return (best);
}

bool        AlphaBetaCustom::_times_up(void) {
    if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->_search_start).count() >= this->_search_limit_ms) {
        this->search_stopped = true;
        return (true);
    }
    return (false);
}

int         AlphaBetaCustom::_elapsed_ms(void) {
    return (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->_search_start).count());
}

/******************************************************** MCTS ********************************************************/

MCTS::MCTS(int depth, uint8_t verbose, int time_max) : AIPlayer(depth, verbose), _time_max(time_max) {
}

MCTS::MCTS(MCTS const &src) : AIPlayer(src.get_depth(), src.get_verbose()) {
    *this = src;
}

MCTS::~MCTS(void) {
}

MCTS   &MCTS::operator=(MCTS const&) {
    return(*this);
}

t_ret const MCTS::operator()(t_node root) {
    return (this->mtcs(root));
}

t_ret       MCTS::mtcs(t_node root) {
    MCTSNode  root_node(root, NULL, 0, 0, 0);
    this->_start = std::chrono::steady_clock::now();    

    std::cout << "--- MCTS ---" << std::endl;
    while (this->timesup()) {
        /* Select */
        MCTSNode    promising_node = this->select_promising_node(root_node);
        std::cout << promising_node << std::endl;
        /* Expand */
        // if (promising_node.get_childs().empty()) {
            // expand_node(node);
        // }
        // /* Simulate */
        // while (node.get_moves()) { // tant que je peux dérouler les mouvs (pas un terminal)
        //     apply_move(); // itère sur les mouvs
        // }
        /* Backpropagate */

    }
    return ((t_ret){ 0, 0 });
}

static double       get_value(int total_visit, double node_wins, int node_visit) {
    if (node_visit == 0)
        return (std::numeric_limits<double>::max());
    return ((node_wins / (double)node_visit) + 1.41 * sqrt(log(total_visit) / node_visit));
}

static MCTSNode     get_best_node_with_uct(MCTSNode node) {
    MCTSNode        *best_node;
    MCTSNode        *current_node;
    int             parent_total_visit = node.get_visit();
    double          temp_uct;
    double          best_uct = std::numeric_limits<double>::min();

    for (size_t i = 0; i < node.get_childs().size(); ++i) {
        current_node = &(node.get_childs()[i]);
        temp_uct = get_value(parent_total_visit, (*current_node).get_wins(), (*current_node).get_visit());
        if (temp_uct > best_uct) {
            best_uct = temp_uct;
            best_node = &node;
        }
    }
    return (*best_node);
}

MCTSNode        MCTS::select_promising_node(MCTSNode root) {
    MCTSNode    node = root;
    while (!node.get_childs().empty()) { // While node is not a leaf (because a leaf is a non-developped node, thus no childs yet)
        node = get_best_node_with_uct(node);
    }
    return (node);
}

bool            MCTS::timesup(void) {
    if ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->_start)).count() <= this->_time_max) {
        return (true);
    }
    return (false);
}







MCTSNode::MCTSNode(t_node node, MCTSNode *parent, int move, int wins, int visit): _node(node), _parent(parent), _move(move), _wins(wins), _visit(visit) {
}

MCTSNode::MCTSNode(MCTSNode const &src) {
    *this = src;
}

MCTSNode::~MCTSNode(void) {
}

MCTSNode	&MCTSNode::operator=(MCTSNode const &) {
    // this->node = rhs.get_node();
    // this->parent = rhs.get_parent();
    // this->move = rhs.get_move();
    // this->wins = rhs.get_wins();
    // this->visit = rhs.get_visit();
    return (*this);
}

std::ostream &operator<<(std::ostream &o, const MCTSNode &rhs) {
    std::stringstream   ss;

    if (rhs.get_parent() != NULL)
        ss << "Node [ parent\t= " << std::to_string(rhs.get_parent()->get_move()) << "\t]\n";
    else
        ss << "Node [ root node ]\n";
    ss << "     [ move\t= " << std::to_string(rhs.get_move()) << "\t]\n";
    ss << "     [ wins\t= " << std::to_string(rhs.get_wins()) << "\t]\n";
    ss << "     [ visit\t= " << std::to_string(rhs.get_visit()) << "\t]";
    o << ss.str();
	return o;
}
