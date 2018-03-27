#ifndef AIALGORITHM_HPP
# define AIALGORITHM_HPP

# include "AIPlayer.hpp"

class MinMax: public AIPlayer {

public:
    MinMax(void);
    MinMax(MinMax const &src);
    ~MinMax(void);
    MinMax    &operator=(MinMax const &rhs);

    virtual t_best  getmove(t_node node, int depth);

private:
    t_best          minmax(t_node node, int depth, int player);

};

class AlphaBeta: public AIPlayer {

public:
    AlphaBeta(void);
    AlphaBeta(AlphaBeta const &src);
    ~AlphaBeta(void);
    AlphaBeta    &operator=(AlphaBeta const &rhs);

    virtual t_best  getmove(t_node node, int depth);

private:
    t_best          alphabeta(t_node, int depth, int alpha, int beta, int player);

};

class NegaScout: public AIPlayer {

public:
    NegaScout(void);
    NegaScout(NegaScout const &src);
    ~NegaScout(void);
    NegaScout    &operator=(NegaScout const &rhs);

    virtual t_best  getmove(t_node node, int depth);

private:
    t_best          negascout(t_node, int depth, int alpha, int beta, int color, int maxdepth);

};


class MTDf: public AIPlayer {

public:
    MTDf(void);
    MTDf(MTDf const &src);
    ~MTDf(void);
    MTDf    &operator=(MTDf const &rhs);

    virtual t_best  getmove(t_node node, int depth);

private:
    t_best          alphabetawithmemory(t_node, int depth, int alpha, int beta, int player);
    t_best          mtdf(t_node node, t_best f, int depth);
    t_best          iterativedeepening(t_node node, int maxdepth);

};

#endif
