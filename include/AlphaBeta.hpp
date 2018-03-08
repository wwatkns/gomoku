#ifndef ALPHABETA_HPP
# define ALPHABETA_HPP

# include <iostream>
# include <cstdlib> // cmath also ?
# include <vector>
# include <Eigen/Dense>
# include <algorithm>
# include <cmath>
# include <limits>
# include "BitBoard.hpp"

/*  Optimizations ideas :
    - bitboards for players to check patterns with bitwise operations blazingly fast
    - rotated bitboards to check the diagonals faster
    - optimized get_open_moves function (with bitboards)
    - heuristic function to set a score on each cell of the board to reduce the search space (with threshold)
    - hash function to access score associated with board state already explored (https://en.wikipedia.org/wiki/Zobrist_hashing)
    - multithreading implementation of minMax on CPU or GPU
    - MTD-f (with transposition tables)
    - iterative deepening IDDFS
*/

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

Eigen::Array2i  alphabeta_pruning(t_node *root, int32_t alpha, int32_t beta, int8_t depth);

int32_t         max(t_node *node, int32_t alpha, int32_t beta, int8_t depth);
int32_t         min(t_node *node, int32_t alpha, int32_t beta, int8_t depth);

int32_t         score_function(t_node *node, uint8_t depth);
bool            check_end(BitBoard const& player, BitBoard const& opponent, uint8_t const& player_pairs_captured, uint8_t const& opponent_pairs_captured);

void            simulate_move(t_node *node, uint16_t i);

int32_t         min_val(int32_t const &a, int32_t const &b);
int32_t         max_val(int32_t const &a, int32_t const &b);

#endif
