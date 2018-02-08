#include "Computer.hpp"

Computer::Computer(GameEngine *game_engine, unsigned short id) : Player(game_engine, id) {
}

Computer::Computer(Computer const &src) : Player(src) {
    *this = src;
}

Computer::~Computer(void) {
}

Computer	&Computer::operator=(Computer const &src) {
    this->_game_engine = src.get_game_engine();
    this->_id = src.get_id();
    return (*this);
}

t_action    Computer::play(void) {
    t_action    action;

    /*  Perform a MinMax and return the most favorable action
    */
    return action;
}
