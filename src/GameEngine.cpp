#include "GameEngine.hpp"

GameEngine::GameEngine(void) {
    this->_grid = Eigen::ArrayXXi::Constant(BOARD_COLS, BOARD_ROWS, state::free);
    this->_initial_timestamp = std::time(nullptr);
}

GameEngine::GameEngine(GameEngine const &src) {
    *this = src;
}

GameEngine::~GameEngine(void) {
}

GameEngine	&GameEngine::operator=(GameEngine const &src) {
    this->_grid = src.get_grid();
    return (*this);
}

/* Getters */
Eigen::ArrayXXi    GameEngine::get_grid(void) const { return (this->_grid); }
/* Setters */
void        GameEngine::set_grid(Eigen::ArrayXXi grid) { this->_grid = grid; }


bool    GameEngine::check_action(t_action &action) {
    return false;
}

bool    GameEngine::check_end(void) {
    return false;
}

void    GameEngine::update_game_state(t_action &action) {
    this->_grid(action.pos[0], action.pos[1]) = (action.player_id == 0 ? state::black : state::white);
    action.timestamp = std::time(nullptr) - this->_initial_timestamp;
    this->_history.push_back(action);
}
