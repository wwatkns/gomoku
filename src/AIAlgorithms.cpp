#include "AIAlgorithms.hpp"
#include "GameEngine.hpp"

#include <chrono>
#include <thread>
#include <stdio.h>

#define SLEEP(X)     std::this_thread::sleep_for(std::chrono::milliseconds(X));

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
    return (this->mcts(root));
}

void        MCTS::debugchilds(MCTSNode node, int level) {
    if (node.get_parent() == NULL)
        std::cout << node << std::endl;
    std::vector<MCTSNode *>   childs = node.get_childs();
    for (std::vector<MCTSNode *>::const_iterator child = childs.begin(); child != childs.end(); ++child) {
        for (int i = 0; i < level; ++i) {
            std::cout << "--";
        }
        std::cout << **child << std::endl;
        if ((*child)->get_childs().size() != 0) {
            debugchilds(**child, level + 1);
        }
    }
    return ;
}

t_ret       MCTS::mcts(t_node root_state) {
    MCTSNode    root_node(NULL, 0, 0, 0, this->move_generation(root_state, 1));
    MCTSNode    *node_to_explore = &root_node;
    MCTSNode    *promising_node;
    t_node      state;
    int         winner;
    this->_start = std::chrono::steady_clock::now();

    while (this->timesup()) {
        state = root_state;
        /* Tree Policy starts here: */
        /* Select */
        promising_node = this->select_promising_node(node_to_explore, state);
        /* Expand */
        if (!((*promising_node).get_untried_actions().empty())) {
            this->expand_node((*promising_node), state);
        }
        /* Default Policy: */
        /* Simulate */
        if (!((*promising_node).get_childs().empty())) {
            node_to_explore = this->get_random_node((*promising_node));
            state = this->create_child(state, (*node_to_explore).get_move());
        }
        winner = this->rollout(state);
        /* Backpropagate */
        node_to_explore = this->backpropagate(node_to_explore, winner);
    }
    return (this->get_best_move(root_node));
}

static double       get_value(int total_visit, double node_wins, int node_visit) {
    if (node_visit == 0)
        return (std::numeric_limits<double>::max());
    return ((node_wins / (double)node_visit) + 0.2 * sqrt(2 * log(total_visit) / node_visit));
}

static MCTSNode     *get_best_node_with_uct(std::vector<MCTSNode *> childs, int parent_total_visit) {
    double          temp_uct;
    double          best_uct = std::numeric_limits<double>::min();
    MCTSNode        *best_node = childs[0];
        
    for (std::vector<MCTSNode *>::iterator child = childs.begin(); child != childs.end(); ++child) {
        temp_uct = get_value(parent_total_visit, (*child)->get_wins(), (*child)->get_visit());
        if (temp_uct > best_uct) {
            best_uct = temp_uct;
            best_node = *child;
        }
    }
    return (best_node);
}

MCTSNode            *MCTS::select_promising_node(MCTSNode *root, t_node &state) {
    MCTSNode        *node = root;
    while (!(node->get_childs().empty()) && node->get_untried_actions().empty()) {
        node = get_best_node_with_uct(node->get_childs(), node->get_visit());
        state = this->create_child(state, node->get_move());
    }
    return (node);
}

void            MCTS::expand_node(MCTSNode &root, t_node &state) {
    std::random_device  random_device;
    std::mt19937        engine{random_device()};
    std::uniform_int_distribution<int>  dist(0, root.get_untried_actions().size() - 1);
    int     random = dist(engine);

    t_move move = root.get_untried_actions()[random];
    state = this->create_child(state, move.p);

    MCTSNode *child = new MCTSNode(&root, move.p, 0, 0, this->move_generation(state, 1));

    root.add_child(child);
    root.remove_action(random);

    return ;
}

MCTSNode         *MCTS::get_random_node(MCTSNode &node) {
    std::random_device  random_device;
    std::mt19937        engine{random_device()};
    std::vector<MCTSNode *> childs = node.get_childs();
    std::uniform_int_distribution<int>  dist(0, childs.size() - 1);
    return (childs[dist(engine)]);
}

int             MCTS::MCTS_check_end(t_node *state) {
    /* Check if one state give and end game and return 0 for none, 1 for player 1 win, 2 for player 2 win */
    if (state->cid == 1 && this->checkEnd(*state)) {
        return (1);
    }
    else if (state->cid == 2 && this->checkEnd(*state)) {
        return (2);
    }
    else {
        return (0);
    }
}

int             MCTS::rollout(t_node state) {
    t_node      tmp_state = state;
    /* Check if the state is final or not */
    int         game_status = MCTS_check_end(&tmp_state);
    /* Generate random move */
    std::random_device  random_device;
    std::mt19937        engine{random_device()};
    std::vector<t_move> moves;
    t_move              select_move;

    while (game_status == 0) {
        /* Generate all legal moves from one node/state */
        moves = this->move_generation(tmp_state, 1);
        /* Select one random move from this vector list */
        std::uniform_int_distribution<int>  dist(0, moves.size() - 1);
        select_move = moves[dist(engine)];
        tmp_state = this->create_child(tmp_state, select_move.p);
        /* Check if this move terminate the play */
        game_status = MCTS_check_end(&tmp_state);
    }
    return (game_status);
}

MCTSNode        *MCTS::backpropagate(MCTSNode *leaf, int winner) {
    MCTSNode    *node_tmp = leaf;
    while (true) {
        node_tmp->inc_visit();
        if (node_tmp->get_playerid() == winner) {
            node_tmp->inc_wins(1);
        }
        if (node_tmp->get_parent() == NULL) {
            break;
        }
        node_tmp = node_tmp->get_parent();
    }
    return (node_tmp);
}

t_ret           MCTS::get_best_move(MCTSNode &root_node) {
    std::vector<MCTSNode *> childs = root_node.get_childs();
    MCTSNode                *best_node = childs[0];
    for (std::vector<MCTSNode *>::const_iterator child = childs.begin(); child != childs.end(); ++child) {
        if ((*child)->get_visit() > best_node->get_visit()) {
            best_node = *child;
        }
    }
    return ((t_ret){ 0, best_node->get_move() });
}

bool            MCTS::timesup(void) {
    if ((std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->_start)).count() <= this->_time_max) {
        return (true);
    }
    return (false);
}

MCTSNode::MCTSNode(MCTSNode *parent, int move, int wins, int visit, std::vector<t_move> moves): _parent(parent), _move(move), _wins(wins), _visit(visit) {
    this->_untried_actions = moves;
    if (this->_parent != NULL && this->_parent->get_playerid() == 1) {
        this->_playerid = 2;
    } else {
        this->_playerid = 1;
    }
}

MCTSNode::MCTSNode(MCTSNode const &src) {
    *this = src;
}

MCTSNode::~MCTSNode(void) {
    for (MCTSNode* child : _childs) {
        delete child;
    }
}

MCTSNode	&MCTSNode::operator=(MCTSNode const &rhs) {
    this->_parent = rhs.get_parent();
    this->_playerid = rhs.get_playerid();
    this->_move = rhs.get_move();
    this->_wins = rhs.get_wins();
    this->_visit = rhs.get_visit();
    this->_childs = rhs.get_childs();
    this->_untried_actions = rhs.get_untried_actions();
    return (*this);
}

void        MCTSNode::add_child(MCTSNode *node) {
    this->_childs.push_back(node);
    return ;
}

void        MCTSNode::remove_action(int index) {
    this->_untried_actions.erase(this->_untried_actions.begin() + index);
    return ;
}


std::ostream &operator<<(std::ostream &o, const MCTSNode &rhs) {
    std::stringstream   ss;

    if (rhs.get_parent() != NULL)
        ss << "Node [ parent\t= " << std::to_string(rhs.get_parent()->get_move()) << "\t]\n";
    else
        ss << "Node [ root node ]\n";
    ss << "     [ player\t= " << std::to_string(rhs.get_playerid()) << "\t]\n";
    ss << "     [ move\t= " << std::to_string(rhs.get_move()) << "\t]\n";
    ss << "     [ wins\t= " << std::to_string(rhs.get_wins()) << "\t]\n";
    ss << "     [ visit\t= " << std::to_string(rhs.get_visit()) << "\t]\n";
    if (rhs.get_childs().size() > 0)
        ss << "     [ childs\t= " << std::to_string(rhs.get_childs().size()) << "\t]\n";
    else
        ss << "     [ childs\t= 0" << "\t]\n";
    if (rhs.get_untried_actions().size() > 0)
        ss << "     [ actions\t= " << std::to_string(rhs.get_untried_actions().size()) << "\t]";
    else
        ss << "     [ actions\t= 0" << "\t]";
    o << ss.str();
	return o;
}
