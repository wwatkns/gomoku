#ifndef ALPHABETA_HPP
# define ALPHABETA_HPP

# include <iostream>
# include <cstdlib> // cmath also ?
# include <vector>
# include <Eigen/Dense>
# include <limits>

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

class AlphaBeta {

public:
    AlphaBeta(GameEngine *game_engine, uint8_t depth);
    AlphaBeta(AlphaBeta const &src);
    ~AlphaBeta(void);
    AlphaBeta	&operator=(AlphaBeta const &rhs);

    uint8_t         get_depth(void) const { return _depth; };
    void            set_depth(uint8_t depth) { _depth = depth; };

      alphabeta_pruning(Player const& player1, Player const& player2);

private:
    GameEngine          *_game_engine;
    uint8_t             _depth;
    Bitboard::bitboard  p1;
    Bitboard::bitboard  p2;

    // int32_t                     _min(t_state game_state, uint8_t current_depth);
    // int32_t                     _max(t_state game_state, uint8_t current_depth);
    // int32_t                     _score(t_state game_state);
    // std::vector<Eigen::Array2i> _get_around_stone_moves(t_state game_state);
    std::vector<Eigen::Array2i> _get_open_moves(t_state game_state);
    // std::vector<Eigen::Array2i> _get_around_stone_moves(t_state game_state);

};

#endif
