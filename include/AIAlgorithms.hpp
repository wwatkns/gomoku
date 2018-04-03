#ifndef AIALGORITHM_HPP
# define AIALGORITHM_HPP

# include <math.h>
# include <random>
# include <iostream>
# include "AIPlayer.hpp"

class MinMax: public AIPlayer {

public:
    MinMax(int depth, uint8_t verbose = verbose::quiet);
    MinMax(MinMax const &src);
    ~MinMax(void);
    MinMax    &operator=(MinMax const &);

    virtual t_ret const operator()(t_node root);

private:
    t_ret           minmax(t_node node, int depth, int player);

};

class AlphaBeta: public AIPlayer {

public:
    AlphaBeta(int depth, uint8_t verbose = verbose::quiet);
    AlphaBeta(AlphaBeta const &src);
    ~AlphaBeta(void);
    AlphaBeta    &operator=(AlphaBeta const &);

    virtual t_ret const operator()(t_node root);

private:
    t_ret           alphabeta(t_node, int depth, int alpha, int beta, int player);

};

class NegaScout: public AIPlayer {

public:
    NegaScout(int depth, uint8_t verbose = verbose::quiet);
    NegaScout(NegaScout const &src);
    ~NegaScout(void);
    NegaScout    &operator=(NegaScout const &);

    virtual t_ret const operator()(t_node root);

private:
    t_ret           negascout(t_node, int depth, int alpha, int beta, int color, int maxdepth);

};


class MTDf: public AIPlayer {

public:
    MTDf(int depth, uint8_t verbose = verbose::quiet);
    MTDf(MTDf const &src);
    ~MTDf(void);
    MTDf    &operator=(MTDf const &);

    virtual t_ret const operator()(t_node root);

private:
    t_ret           alphabetawithmemory(t_node, int depth, int alpha, int beta, int player);
    t_ret           mtdf(t_node node, t_ret  f, int depth);
    t_ret           iterativedeepening(t_node node, int maxdepth);
    bool            timesup(void);
    std::chrono::steady_clock::time_point start;

};

/*  + Alpha-Beta pruning
    + Iterative Deepening
    + Move Sorting
*/
class AlphaBetaCustom: public AIPlayer {

public:
    AlphaBetaCustom(int depth, uint8_t verbose = verbose::quiet, int time_limit = 500);
    AlphaBetaCustom(AlphaBetaCustom const &src);
    ~AlphaBetaCustom(void);
    AlphaBetaCustom	&operator=(AlphaBetaCustom const &rhs);

    // int         get_max_depth(void) const { return (_max_depth); };
    int         get_search_limit_ms(void) const { return (_search_limit_ms); };

    virtual t_ret const operator()(t_node root);
    // t_ret const operator()(t_node root);

    bool    search_stopped;

private:
    int                                     _current_max_depth;
    std::chrono::steady_clock::time_point   _search_start;
    int                                     _search_limit_ms;
    std::vector<t_move>                     _root_moves;

    t_ret                                   _root_max(t_node node, int alpha, int beta, int depth);
    t_ret                                   _max(t_node node, int alpha, int beta, int depth);
    t_ret                                   _min(t_node node, int alpha, int beta, int depth);

    bool                                    _times_up(void);
    int                                     _elapsed_ms(void);
};

// namespace MCTS {

// MAX_ROLLOUT = 50;

// struct  simple_random {
//     double operator()() {
//         return (static_cast<double>(rand() % RAND_MAX) / RAND_MAX)
//     }
// }

// // template<int N_ACTIONS, class URand = simple_random> class MCTSTreeNode {
// class MCTSTreeNode {

// private:
//     bool                is_leaf;
//     int                 visited;
//     int                 wins;
//     int                 total_value;
//     std::vector<t_move> child_nodes;

//     int                 select(void);

// }

// }


// class MCTSNode: {

// public:
//     MCTSNode(int depth, uint8_t verbose = verbose::quiet, int time_max = 500);
//     MCTSNode(MCTSNode const &src);
//     ~MCTSNode(void);
//     MCTSNode	&operator=(MCTSNode const &rhs);

//     int         win;
//     int         visit;
//     int         move;
//     MCTSNode    *parent;
//     std::vector<MCTSNode> childs;

// };

class MCTSNode {

public:
    MCTSNode(MCTSNode *parent, int move, int wins, int visit);
    MCTSNode(MCTSNode const &src);
    ~MCTSNode(void);
    MCTSNode	&operator=(MCTSNode const &rhs);

    int                     get_visit(void) const { return (this->_visit); };
    int                     get_wins(void) const { return (this->_wins); };
    int                     get_move(void) const { return (this->_move); };
    int                     get_playerid(void) const { return (this->_playerid); };
    // t_node                  get_node(void) const { return (this->_node); };
    MCTSNode                *get_parent(void) const { return (this->_parent); };
    std::vector<MCTSNode>   get_childs(void) const { return (this->_childs); };
    std::vector<t_move>     get_untried_actions(void) const { return (this->_untried_actions); };

    void                    set_wins(int value) { this->_wins = value; return ; };
    void                    set_untried_actions(std::vector<t_move> moves) { this->_untried_actions = moves; return ;};
    void                    inc_wins(int score) { this->_wins += score; return ; };
    void                    inc_visit(void) { this->_visit++; return ; };

    void                    add_child(MCTSNode node);
    void                    remove_action(int index);

private:
    // t_node                  _node;    // state
    MCTSNode                *_parent; // parent node
    int                     _playerid;// player ID
    int                     _move;    // position played to reach the state contained in node
    int                     _wins;    // number of wins
    int                     _visit;   // number of visitation
    std::vector<MCTSNode>   _childs;  // vector of childs nodes
    std::vector<t_move>     _untried_actions;

};

std::ostream &operator<<(std::ostream &o, const MCTSNode &rhs);

class MCTS: public AIPlayer {

public:
    MCTS(int depth, uint8_t verbose = verbose::quiet, int time_max = 1000);
    MCTS(MCTS const &src);
    ~MCTS(void);
    MCTS	&operator=(MCTS const &rhs);

    virtual t_ret const operator()(t_node root);

private:
    t_ret       mtcs(t_node root_state); // Enchaine les 4 phases de MCTS

    MCTSNode    select_promising_node(MCTSNode &root, t_node &state); // Select phase
    void        expand_node(MCTSNode &root, t_node &state); // Expand phase
    MCTSNode    get_random_node(MCTSNode node); // Roll out or simulation phase
    int         rollout(MCTSNode node, t_node state); // Roll out
    // t_node      randomize_and_apply(std::vector<t_move> moves);
    MCTSNode    backpropagate(MCTSNode leaf, int winner);
    t_ret       get_best_move(MCTSNode root_node);

    // std::random_device  _random_device;
    // std::mt19937        _engine{_random_device()};


    // double      uct_value(int totalvisits, int nodevisit, double nodewin);
    // void        expand(t_node node);
    // void        backpropagation();
    // int         simulate_random_playout();

    std::chrono::steady_clock::time_point _start;
    int         _time_max;
    bool        timesup(void);

};

#endif
