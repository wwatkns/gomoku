#ifndef MTDF_HPP
# define MTDF_HPP

# include <iostream>
# include <cstdlib> // cmath also ?
# include <vector>
# include <Eigen/Dense>
# include <algorithm>
# include <cmath>
# include <utility>
# include "BitBoard.hpp"
// # include "ZobristTable.hpp"

# define INF 2147483647

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
    int32_t     score;
    uint16_t    p;
}               t_ret;

bool            times_up(std::chrono::steady_clock::time_point start, uint32_t limit);

Eigen::Array2i  iterative_deepening(t_node *root, int8_t max_depth);
t_ret           mtdf(t_node *root, int32_t firstguess, int8_t depth);
t_ret           alphaBetaWithMemory(t_node root, int32_t alpha, int32_t beta, int8_t depth);

int32_t         max(t_node *node, int32_t alpha, int32_t beta, int8_t depth);
int32_t         min(t_node *node, int32_t alpha, int32_t beta, int8_t depth);

int32_t         score_function(t_node *node, uint8_t depth);
int32_t         player_score(t_node *node, uint8_t depth);
int32_t         opponent_score(t_node *node, uint8_t depth);
bool            check_end(BitBoard const& player, BitBoard const& opponent, uint8_t const& player_pairs_captured, uint8_t const& opponent_pairs_captured);

void            simulate_move(t_node *node, uint16_t i);

int32_t         min_val(int32_t const &a, int32_t const &b);
int32_t         max_val(int32_t const &a, int32_t const &b);

void            TT_store(t_node *node, int32_t best, int32_t alpha, int32_t beta, int8_t depth);
int32_t         TT_lookup(t_node *node, int32_t alpha, int32_t beta, int8_t depth);

#endif
