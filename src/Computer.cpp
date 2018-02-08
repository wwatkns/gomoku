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
    action.pos = {0, 0}; // TMP
    action.player_id = this->_id;
    action.num = this->_game_engine->get_history_size() + 1;
    action.timepoint = std::chrono::steady_clock::now() - this->_game_engine->get_initial_timepoint();
    return action;
}
