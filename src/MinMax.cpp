#include "MinMax.hpp"
#include "Player.hpp"

MinMax::MinMax(uint8_t depth) : _depth(depth) {
}

MinMax::MinMax(MinMax const &src) {
    *this = src;
}

MinMax::~MinMax(void) {
}

MinMax	&MinMax::operator=(MinMax const &src) {
    return (*this);
}

Eigen::Array2i  MinMax::minmax(Eigen::ArrayXXi game_state, Player *player) {
    std::vector<Eigen::Array2i> pos = this->_get_open_moves(game_state, player);

    for (std::vector<Eigen::Array2i>::iterator it = pos.begin(); it != pos.end(); ++it) {
        std::cout << (*it)[0] << "," << (*it)[1] << " " << std::endl; // [0] is J, [1] is I
    }
    return pos[0];
}


std::vector<Eigen::Array2i> MinMax::_get_open_moves(Eigen::ArrayXXi game_state, Player *player) {
    std::vector<Eigen::Array2i> open_moves;

    for (uint32_t i = 0; i < game_state.size(); i++) {
        if (*(game_state.data()+i) == 0 || *(game_state.data()+i) == (player->get_id() == 1 ? 10 : -10))
            open_moves.push_back((Eigen::Array2i){ i % game_state.rows(), i / game_state.rows() });
    }
    return open_moves;
}
