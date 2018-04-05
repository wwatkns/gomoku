#include "Player.hpp"

Player::Player(GameEngine *game_engine, GraphicalInterface *gui, uint8_t id, int algo_type, int depth) : _game_engine(game_engine), _gui(gui), _id(id), _pairs_captured(0) {
    this->suggested_move = { -1, -1 };
    this->current_score = 0;
    if (algo_type == 2)
        this->_ai_algorithm = (AIPlayer*)new MinMax(depth, id, verbose::quiet);
    else if (algo_type == 3)
        this->_ai_algorithm = (AIPlayer*)new AlphaBeta(depth, id, verbose::quiet);
    else if (algo_type == 4)
        this->_ai_algorithm = (AIPlayer*)new MTDf(depth, id, verbose::quiet);
    else if (algo_type == 5)
        this->_ai_algorithm = (AIPlayer*)new MCTS(depth, id, verbose::quiet);
    else
        this->_ai_algorithm = (AIPlayer*)new AlphaBetaCustom(depth, id, verbose::quiet);
}

Player::Player(Player const &src) : _game_engine(src.get_game_engine()), _id(src.get_id()) {
    *this = src;
}

Player	&Player::operator=(Player const &src) {
    this->_game_engine = src.get_game_engine();
    this->_id = src.get_id();
    this->_ai_algorithm = src.get_ai_algorithm();
    return (*this);
}
