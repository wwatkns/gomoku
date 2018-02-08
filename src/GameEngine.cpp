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
    return (*this);
}

bool    GameEngine::check_action(t_action &action) {
    return false;
}

bool    GameEngine::check_end(void) {
    for (size_t col = 0; col < BOARD_ROWS; col++) {
        for (size_t row = 0; row < BOARD_COLS; row++) {
            if (this->_grid(row, col) == state::black || this->_grid(row, col) == state::white) {
                if (_check_col(col, row) || _check_row(col, row) ||
                    _check_dil(col, row) || _check_dir(col, row))
                    return true;
            }
        }
    }
    return false;
}

void    GameEngine::update_game_state(t_action &action) {
    this->_grid(action.pos[0], action.pos[1]) = (action.player_id == 0 ? state::black : state::white);
    this->_history.push_back(action);
}

bool    GameEngine::_check_col(size_t col, size_t row) {
    int sum = 0;

    if (row <= (BOARD_ROWS - ALIGNTOWIN)) { // row <= 14
        sum = this->_grid(row,     col) + this->_grid(row + 1, col) +
              this->_grid(row + 2, col) + this->_grid(row + 3, col) +
              this->_grid(row + 4, col);
        if (sum == ALIGNTOWIN || sum == -ALIGNTOWIN) {
            return true;
        }
    }
    return false;
}

bool    GameEngine::_check_row(size_t col, size_t row) {
    int sum = 0;

    if (col <= (BOARD_COLS - ALIGNTOWIN)) { // 0 <= cols <= 14
        sum = this->_grid(row, col    ) + this->_grid(row, col + 1) +
              this->_grid(row, col + 2) + this->_grid(row, col + 3) +
              this->_grid(row, col + 4);
        if (sum == ALIGNTOWIN || sum == -ALIGNTOWIN) {
            return true;
        }
    }
    return false;
}

/* Check diagonal left */
bool    GameEngine::_check_dil(size_t col, size_t row) {
    int sum = 0;

    if (col >= (ALIGNTOWIN - 1) && row <= (BOARD_ROWS - ALIGNTOWIN)) {
        sum = this->_grid(row    , col    ) + this->_grid(row + 1, col - 1) +
              this->_grid(row + 2, col - 2) + this->_grid(row + 3, col - 3) +
              this->_grid(row + 4, col - 4);
        if (sum == ALIGNTOWIN || sum == -ALIGNTOWIN) {
            return true;
        }
    }
    return false;
}

/* Check diagonal right */
bool    GameEngine::_check_dir(size_t col, size_t row) {
    int sum = 0;

    if (col <= (BOARD_COLS - ALIGNTOWIN) && row <= (BOARD_ROWS - ALIGNTOWIN)) {
        sum = this->_grid(row    , col    ) + this->_grid(row + 1, col + 1) +
              this->_grid(row + 2, col + 2) + this->_grid(row + 3, col + 3) +
              this->_grid(row + 4, col + 4);
        if (sum == ALIGNTOWIN || sum == -ALIGNTOWIN) {
            return true;
        }
    }
    return false;
}

/* Getters */
Eigen::ArrayXXi                         GameEngine::get_grid(void) const { return (this->_grid); }
std::list<t_action>                     GameEngine::get_history(void) const { return (this->_history); }
unsigned long                           GameEngine::get_history_size(void) const { return (this->_history.size()); }
std::chrono::steady_clock::time_point   GameEngine::get_initial_timepoint(void) const { return (this->_initial_timepoint); }
/* Setters */
void                                    GameEngine::set_grid(Eigen::ArrayXXi grid) { this->_grid = grid; }
