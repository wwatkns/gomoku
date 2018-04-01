#ifndef AIALGORITHM_HPP
# define AIALGORITHM_HPP

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

    int         get_search_limit_ms(void) const { return (_search_limit_ms); };

    virtual t_ret const operator()(t_node root);

    bool    search_stopped;

private:
    int                                     _current_max_depth;
    std::chrono::steady_clock::time_point   _search_start;
    int                                     _search_limit_ms;
    std::vector<t_move>                     _root_moves;

    t_ret                                   _root_max(t_node node, int alpha, int beta, int depth);
    t_ret                                   _max(t_node node, int alpha, int beta, int depth);
    t_ret                                   _min(t_node node, int alpha, int beta, int depth);

    void                                    _debug_append_explored(int score, int i, int depth);
    void                                    _debug_search(t_ret const& ret);

    bool                                    _times_up(void);
    int                                     _elapsed_ms(void);
};

#endif
