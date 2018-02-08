#include "GameEngine.hpp"

GameEngine::GameEngine(void) {
    this->_grid = Eigen::ArrayXXi::Constant(BOARD_COLS, BOARD_ROWS, state::free);
    this->_initial_timepoint = std::chrono::steady_clock::now();
}

GameEngine::GameEngine(GameEngine const &src) {
    *this = src;
}

GameEngine::~GameEngine(void) {
}

GameEngine	&GameEngine::operator=(GameEngine const &src) {
    this->_grid = src.get_grid();
    this->_history = src.get_history();
    this->_initial_timepoint = src.get_initial_timepoint();
    return (*this);
}

bool    GameEngine::check_action(t_action &action) {
    return false;
}

bool    GameEngine::check_end(void) {
    return false;
}

void    GameEngine::update_game_state(t_action &action) {
    this->_grid(action.pos[0], action.pos[1]) = (action.player_id == 0 ? state::black : state::white);
    this->_history.push_back(action);
}


/* Getters */
Eigen::ArrayXXi                         GameEngine::get_grid(void) const { return (this->_grid); }
std::list<t_action>                     GameEngine::get_history(void) const { return (this->_history); }
unsigned long                           GameEngine::get_history_size(void) const { return (this->_history.size()); }
std::chrono::steady_clock::time_point   GameEngine::get_initial_timepoint(void) const { return (this->_initial_timepoint); }
/* Setters */
void                                    GameEngine::set_grid(Eigen::ArrayXXi grid) { this->_grid = grid; }
