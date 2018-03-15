#ifndef MTDF_HPP
# define MTDF_HPP

# include <iostream>
# include <Eigen/Dense>
# include "BitBoard.hpp"

# define INF 2147483647
# define EXPLORATION_THRESHOLD 3

class Player;

typedef struct  s_node {
    BitBoard        player;
    BitBoard        player_forbidden;
    BitBoard        opponent;
    BitBoard        opponent_forbidden;
    uint8_t         pid;
    uint8_t         player_pairs_captured;
    uint8_t         opponent_pairs_captured;
}               t_node;

typedef struct  s_ret {
    int         score;
    int         p;
}               t_ret;

t_node          create_node(Player const& player, Player const& opponent);

t_ret           max(t_ret const& a, t_ret const& b);
t_ret           min(t_ret const& a, t_ret const& b);
int             max(int const& a, t_ret const& b);
int             min(int const& a, t_ret const& b);
int             max(int const& a, int const& b);
int             min(int const& a, int const& b);

bool            check_end(t_node const& node);

BitBoard        moves_to_explore(BitBoard const& player, BitBoard const& opponent, BitBoard const& player_forbidden, int player_pairs_captured, int opponent_pairs_captured);
t_ret           alphaBetaWithMemory(t_node node, int alpha, int beta, int depth);
t_ret           alphaBetaMin(t_node node, int alpha, int beta, int depth);
t_ret           alphaBetaMax(t_node node, int alpha, int beta, int depth);

bool            times_up(std::chrono::steady_clock::time_point start, uint32_t limit);

Eigen::Array2i  iterative_deepening(t_node *root, int8_t max_depth);
t_ret           mtdf(t_node *root, int32_t firstguess, int8_t depth);
// t_ret           alphaBetaWithMemory(t_node root, int32_t alpha, int32_t beta, int8_t depth);

// int32_t         max(t_node node, int32_t alpha, int32_t beta, int8_t depth);
// int32_t         min(t_node node, int32_t alpha, int32_t beta, int8_t depth);

int32_t         score_function(t_node const &node, uint8_t depth);
int32_t         player_score(t_node const &node, uint8_t depth);
int32_t         opponent_score(t_node const &node, uint8_t depth);
// bool            check_end(BitBoard const& player, BitBoard const& opponent, uint8_t const& player_pairs_captured, uint8_t const& opponent_pairs_captured);

t_node          simulate_move(t_node const &node, int i);

int32_t         min_val(int32_t const &a, int32_t const &b);
int32_t         max_val(int32_t const &a, int32_t const &b);

void            TT_store(t_node const &node, int32_t best, int32_t alpha, int32_t beta, int8_t depth, int flag);
int32_t         TT_lookup(t_node const &node, int32_t alpha, int32_t beta, int8_t depth);


Eigen::Array2i  iterativeDeepening(t_node root, int max_depth);

class AlphaBeta {

public:
    AlphaBeta(int max_depth, bool use_time_limit = false, int time_limit = 500);
    AlphaBeta(AlphaBeta const &src);
    ~AlphaBeta(void);
    AlphaBeta	&operator=(AlphaBeta const &rhs);

    int         get_max_depth(void) const { return (_max_depth); };
    int         get_search_limit_ms(void) const { return (_search_limit_ms); };

    t_ret const operator()(t_node root, int alpha, int beta, int depth);

    bool    search_stopped;

private:
    int                                     _max_depth;
    std::chrono::steady_clock::time_point   _search_start;
    int                                     _search_limit_ms;
    bool                                    _use_search_limit;

    int                                     _max(t_node const node, int alpha, int beta, int depth);
    int                                     _min(t_node const node, int alpha, int beta, int depth);
    t_node                                  _create_child(t_node const& parent, int m);
    bool                                    _times_up(void); // only use to break search when iterative deepening is used
};

#endif
