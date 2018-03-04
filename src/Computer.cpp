#include "Computer.hpp"
#include <cstdlib> // TMP
#include <thread> // TMP

Computer::Computer(GameEngine *game_engine, uint8_t id) : Player(game_engine, NULL, id) {
    std::srand(std::time(nullptr));
    this->type = 1;
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

bool        Computer::play(Player *other) {
    t_action                                action;
    std::chrono::steady_clock::time_point   action_beg = std::chrono::steady_clock::now();
    Eigen::Array2i                   pos;

    std::this_thread::sleep_for(std::chrono::milliseconds(100 + std::rand() % 900));
    action.timepoint = std::chrono::steady_clock::now() - this->_game_engine->get_initial_timepoint();
    action.duration = std::chrono::steady_clock::now() - action_beg;
    action.pos = {std::rand() % 19, std::rand() % 19}; // TMP

    // pos = this->_game_engine->minmax->minmax(this->_game_engine->grid, this, other);

    action.id = this->_game_engine->get_history_size() + 1;
    action.p1_last = this->board;
    action.p2_last = other->board;
    action.pid = this->_id;
    action.ppc = this->_pairs_captured;
    this->_game_engine->update_game_state(action, this, other);
    return true;
}
