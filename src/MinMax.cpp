#include "MinMax.hpp"
#include "Player.hpp"

MinMax::MinMax(GameEngine *game_engine, uint8_t depth) : _game_engine(game_engine), _depth(depth) {
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
    std::vector<Eigen::Array2i> open_moves = this->_get_open_moves(game_state, player);
    Eigen::ArrayXXi             tmp = game_state;
    uint32_t                    score;
    t_sim                       sim;

    sim.score = std::numeric_limits<unsigned int>::max();
    sim.pos = open_moves[0]; /* if all scores are 0, return first open move */

    /* call max for all possible moves */
    for (std::vector<Eigen::Array2i>::iterator it = open_moves.begin(); it != open_moves.end(); ++it) {
        tmp((*it)[0], (*it)[1]) = (player->get_id() == 1 ? -1 : 1);
        score = this->_min(tmp, 1);
        sim = (score > sim.score ? { *it, score } : sim);
        tmp = game_state;
    }
    return sim.pos;
}


std::vector<Eigen::Array2i> MinMax::_get_open_moves(Eigen::ArrayXXi game_state, Player *player) {
    std::vector<Eigen::Array2i> open_moves;

    for (uint32_t i = 0; i < game_state.size(); i++) {
        if (*(game_state.data()+i) == 0 || *(game_state.data()+i) == (player->get_id() == 1 ? 10 : -10))
            open_moves.push_back((Eigen::Array2i){ i % game_state.rows(), i / game_state.rows() });
    }
    // for (std::vector<Eigen::Array2i>::iterator it = open_moves.begin(); it != open_moves.end(); ++it) {
    //     std::cout << (*it)[0] << "," << (*it)[1] << " " << std::endl; // 0 is J, 1 is I
    // }
    return open_moves;
}

int32_t     MinMax::_min(Eigen::ArrayXXi game_state, uint8_t current_depth) {
    std::vector<Eigen::Array2i> open_moves = this->_get_open_moves(game_state, player);
    Eigen::ArrayXXi             tmp = game_state;
    uint32_t                    score;
    uint32_t                    max;

    if (current_depth == this->_depth || this->_game_engine->check_end()) { /* pairs are fuckboys */
        // compute score
        // return (score);
    }

    max = 0;
    for (std::vector<Eigen::Array2i>::iterator it = open_moves.begin(); it != open_moves.end(); ++it) {
        tmp((*it)[0], (*it)[1]) = (player->get_id() == 1 ? -1 : 1);
        score = this->_max(tmp, current_depth+1);
        max = (score > max ? score : max);
        tmp = game_state;
    }
    return (max);
}

int32_t     MinMax::_max(Eigen::ArrayXXi game_state, uint8_t current_depth) {
    std::vector<Eigen::Array2i> open_moves = this->_get_open_moves(game_state, player);
    Eigen::ArrayXXi             tmp = game_state;
    uint32_t                    score;
    uint32_t                    min;

    if (current_depth == this->_depth || this->_game_engine->check_end()) {
        // compute score
        // return (score);
    }

    min = std::numeric_limits<unsigned int>::max();
    for (std::vector<Eigen::Array2i>::iterator it = open_moves.begin(); it != open_moves.end(); ++it) {
        tmp((*it)[0], (*it)[1]) = (player->get_id() == 1 ? -1 : 1);
        score = this->_min(tmp, current_depth+1);
        min = (score < min ? score : min);
        tmp = game_state;
    }
    return (min);
}


/*
 function minimax(node, depth, maximizingPlayer)
     if depth = 0 or node is a terminal node
         return the heuristic value of node

     if maximizingPlayer
         bestValue := −∞
         for each child of node
             v := minimax(child, depth − 1, FALSE)
             bestValue := max(bestValue, v)
         return bestValue

     else    (* minimizing player *)
         bestValue := +∞
         for each child of node
             v := minimax(child, depth − 1, TRUE)
             bestValue := min(bestValue, v)
         return bestValue
*/
