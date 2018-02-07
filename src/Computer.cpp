#include "Computer.hpp"

Computer::Computer(GameEngine *game_engine, unsigned short index) : Player(game_engine, index) {
}

Computer::Computer(Computer const &src) : Player(src) {
    *this = src;
}

Computer::~Computer(void) {
}

Computer	&Computer::operator=(Computer const &src) {
    this->_game_engine = src.get_game_engine();
    this->_index = src.get_index();
    return (*this);
}

t_action    Computer::play(void) {
    t_action    action;

    /*  Perform a MinMax and return the most favorable action
    */
    return action;
}
