#ifndef NEGAMAX_HPP
# define NEGAMAX_HPP

# include <iostream>
# include <cstdlib> // cmath also ?
# include <vector>
# include <Eigen/Dense>
# include <algorithm>
# include <cmath>
# include <utility>
# include "BitBoard.hpp"
# include "mtdf.hpp"

# define INF 2147483647

/**
 * 
 */

typedef struct      s_ret {
    int             pos;
    int             score;
}                   t_ret;

Eigen::Array2i  negamaxplay(t_node *root, int8_t depth);
// int32_t         negamax_alphabeta(t_node *node, int8_t depth, int8_t color, int alpha, int beta);
t_ret         negamax_alphabeta(t_node *node, int8_t depth, int alpha, int beta, int color);
int32_t         negamax(t_node *node, int8_t depth, int8_t color);

int32_t         negamax_score_function(t_node *node, uint8_t depth, int8_t color);

#endif
