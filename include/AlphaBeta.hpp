#ifndef ALPHABETA_HPP
# define ALPHABETA_HPP

# include <iostream>
# include <cstdlib> // cmath also ?
# include <vector>
# include <Eigen/Dense>
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

typedef struct  s_node {
    BitBoard        p1;
    BitBoard        p1_forbidden;
    BitBoard        p2;
    BitBoard        p2_forbidden;
    uint8_t         current_player_id;
    uint8_t         p1_pairs_captured;
    uint8_t         p2_pairs_captured;
}               t_node;

Eigen::Array2i  alphabeta_pruning(t_node root, uint8_t depth);

uint32_t        max(t_node *node, uint32_t alpha, uint32_t beta, uint8_t current_depth);
uint32_t        min(t_node *node, uint32_t alpha, uint32_t beta, uint8_t current_depth);

uint32_t        score_function(t_node *node);
uint8_t         check_end(BitBoard const& p1, BitBoard const& p2, uint8_t const& p1_pairs_captured, uint8_t const& p2_pairs_captured);

#endif
