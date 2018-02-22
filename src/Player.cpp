#include "Player.hpp"

Player::Player(GameEngine *game_engine, GraphicalInterface *gui, uint8_t id) : _game_engine(game_engine), _gui(gui), _id(id), _pairs_captured(0) {
}

Player::Player(Player const &src) : _game_engine(src.get_game_engine()), _id(src.get_id()) {
    *this = src;
}

Player	&Player::operator=(Player const &src) {
    this->_game_engine = src.get_game_engine();
    this->_id = src.get_id();
    return (*this);
}
