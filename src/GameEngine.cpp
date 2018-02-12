#include "GameEngine.hpp"
#include "Player.hpp"

GameEngine::GameEngine(void) {
    this->grid = Eigen::ArrayXXi::Constant(BOARD_COLS, BOARD_ROWS, state::free);
    this->_initial_timepoint = std::chrono::steady_clock::now();
}

GameEngine::GameEngine(GameEngine const &src) {
    *this = src;
}

GameEngine::~GameEngine(void) {
}

GameEngine	&GameEngine::operator=(GameEngine const &src) {
    this->grid = src.grid;
    this->_history = src.get_history_copy();
    this->_initial_timepoint = src.get_initial_timepoint();
    return (*this);
}

/*
    Check the validity of an action by looking at the cell availability for the current player.
        * -1/1  :   represent a stone already on board
        * -10/10:   non-possible move (because of free-threes for example)
*/
bool    GameEngine::check_action(t_action &action) {
    int x =0, y = 0;

    y = action.pos(0);
    x = action.pos(1);
    if (action.player_id == 1) {
        if (this->grid(y, x) == -1 || this->grid(y, x) == 1 || this->grid(y, x) == -10) {
            return false;
        }
    }
    else {
        if ( this->grid(y, x) == -1 || this->grid(y, x) == 1 || this->grid(y, x) == 10) {
            return false;
        }
    }
    return true;
}

bool    GameEngine::check_end(uint8_t player_pairs) {
    for (size_t col = 0; col < BOARD_ROWS; ++col) {
        for (size_t row = 0; row < BOARD_COLS; ++row) {
            if (this->grid(row, col) == state::black || this->grid(row, col) == state::white) {
                if (_check_col(col, row) || _check_row(col, row) ||
                    _check_dil(col, row) || _check_dir(col, row) ||
                    _check_pairs(player_pairs)) // TODO (alain): est ce que je peux passer un player ?
                    return true;
            }
        }
    }
    return false;
}

/*
*/
void    GameEngine::update_game_state(t_action &action) {
    this->grid(action.pos[0], action.pos[1]) = (action.player_id == 1 ? state::black : state::white);
    this->_history.push_back(action);
}

bool    GameEngine::_check_col(size_t col, size_t row) {
    int sum = 0;

    if (row <= (BOARD_ROWS - ALIGNTOWIN)) { // row <= 14
        sum = this->grid(row,     col) + this->grid(row + 1, col) +
              this->grid(row + 2, col) + this->grid(row + 3, col) +
              this->grid(row + 4, col);
        if (sum == ALIGNTOWIN || sum == -ALIGNTOWIN) {
            return true;
        }
    }
    return false;
}

bool    GameEngine::_check_row(size_t col, size_t row) {
    int sum = 0;

    if (col <= (BOARD_COLS - ALIGNTOWIN)) { // 0 <= cols <= 14
        sum = this->grid(row, col    ) + this->grid(row, col + 1) +
              this->grid(row, col + 2) + this->grid(row, col + 3) +
              this->grid(row, col + 4);
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
        sum = this->grid(row    , col    ) + this->grid(row + 1, col - 1) +
              this->grid(row + 2, col - 2) + this->grid(row + 3, col - 3) +
              this->grid(row + 4, col - 4);
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
        sum = this->grid(row    , col    ) + this->grid(row + 1, col + 1) +
              this->grid(row + 2, col + 2) + this->grid(row + 3, col + 3) +
              this->grid(row + 4, col + 4);
        if (sum == ALIGNTOWIN || sum == -ALIGNTOWIN) {
            return true;
        }
    }
    return false;
}

/* Check pairs captured. If it is == 5: win! */
bool    GameEngine::_check_pairs(uint8_t pairs) {
    if (pairs != 5)
        return false;
    return true;
}
