#ifndef MINMAX_HPP
# define MINMAX_HPP

# include <iostream>
# include <vector>
# include <Eigen/Dense>

class Player;

class MinMax {

public:
    MinMax(uint8_t depth);
    MinMax(MinMax const &src);
    ~MinMax(void);
    MinMax	&operator=(MinMax const &rhs);

    uint8_t         get_depth(void) const { return _depth; };
    void            set_depth(uint8_t depth) { _depth = depth; };

    Eigen::Array2i  minmax(Eigen::ArrayXXi game_state, Player *player);

private:
    uint8_t         _depth;

    int32_t         _min(Eigen::ArrayXXi game_state);
    int32_t         _max(Eigen::ArrayXXi game_state);
    int32_t         _score(Eigen::ArrayXXi game_state);
    std::vector<Eigen::Array2i> _get_open_moves(Eigen::ArrayXXi game_state, Player *player);

};

#endif
