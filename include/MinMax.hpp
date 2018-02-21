#ifndef MINMAX_HPP
# define MINMAX_HPP

# include <iostream>
# include <vector>
# include <Eigen/Dense>
# include <limits>

class Player;
class GameEngine;

typedef struct  s_state {
    Eigen::ArrayXXi grid;
    uint8_t         current_player_id;
    uint8_t         p1_pairs_captured;
    uint8_t         p2_pairs_captured;
}               t_state;

class MinMax {

public:
    MinMax(GameEngine *game_engine, uint8_t depth);
    MinMax(MinMax const &src);
    ~MinMax(void);
    MinMax	&operator=(MinMax const &rhs);

    uint8_t         get_depth(void) const { return _depth; };
    void            set_depth(uint8_t depth) { _depth = depth; };

    Eigen::Array2i  minmax(Eigen::ArrayXXi grid, Player *player);

private:
    GameEngine      *_game_engine;
    uint8_t         _depth;

    int32_t                     _min(t_state game_state, uint8_t current_depth);
    int32_t                     _max(t_state game_state, uint8_t current_depth);
    int32_t                     _score(t_state game_state);
    std::vector<Eigen::Array2i> _get_open_moves(t_state game_state);

};

#endif
