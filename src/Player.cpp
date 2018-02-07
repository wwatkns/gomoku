#include "Player.hpp"

Player::Player(GameEngine *game_engine, unsigned short index) : _game_engine(game_engine), _index(index) {
}

Player::Player(Player const &src) : _game_engine(src.get_game_engine()), _index(src.get_index()) {
    *this = src;
}

Player::~Player(void) {
}

Player	&Player::operator=(Player const &src) {
    this->_game_engine = src.get_game_engine();
    this->_index = src.get_index();
    return (*this);
}

GameEngine      *Player::get_game_engine(void) const { return (this->_game_engine); }
unsigned short  Player::get_index(void) const { return (this->_index); }
