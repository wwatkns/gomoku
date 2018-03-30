#ifndef AIALGORITHM_HPP
# define AIALGORITHM_HPP

# include <math.h>
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


class MCTSNode {

public:
    MCTSNode(t_node node, MCTSNode *parent, int move, int wins, int visit);
    MCTSNode(MCTSNode const &src);
    ~MCTSNode(void);
    MCTSNode	&operator=(MCTSNode const &rhs);

    int                     get_visit(void) const { return (this->_visit); };
    int                     get_wins(void) const { return (this->_wins); };
    int                     get_move(void) const { return (this->_move); };
    t_node                  get_node(void) const { return (this->_node); };
    MCTSNode                *get_parent(void) const { return (this->_parent); };
    std::vector<MCTSNode>   get_childs(void) const { return (this->_childs); };

private:
    t_node                  _node;    // state
    MCTSNode                *_parent; // parent node
    int                     _move;    // position played to reach the state contained in node
    int                     _wins;    // number of wins
    int                     _visit;   // number of visitation
    std::vector<MCTSNode>   _childs;  // vector of childs nodes

};

std::ostream &operator<<(std::ostream &o, const MCTSNode &rhs);

class MCTS: public AIPlayer {

public:
    MCTS(int depth, uint8_t verbose = verbose::quiet, int time_max = 500);
    MCTS(MCTS const &src);
    ~MCTS(void);
    MCTS	&operator=(MCTS const &rhs);

    virtual t_ret const operator()(t_node root);

private:
    t_ret       mtcs(t_node root); // Enchaine les 4 phases de MCTS

    MCTSNode      select_promising_node(MCTSNode root); // Select phase

    std::chrono::steady_clock::time_point _start;
    int         _time_max;
    bool        timesup(void);

};

#endif
