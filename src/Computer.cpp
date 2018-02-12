#include "Computer.hpp"
#include <cstdlib> // TMP
# include <thread> // TMP

Computer::Computer(GameEngine *game_engine, unsigned short id) : Player(game_engine, NULL, id) {
    std::srand(std::time(nullptr));
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
    std::this_thread::sleep_for(std::chrono::milliseconds(100 + std::rand() % 900));
    action.pos = {std::rand() % 19, std::rand() % 19}; // TMP
    action.player_id = this->_id;
    action.player = this;
    action.id = this->_game_engine->get_history_size() + 1;
    action.timepoint = std::chrono::steady_clock::now() - this->_game_engine->get_initial_timepoint();
    return action;
}
