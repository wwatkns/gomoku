#include "GameEngine.hpp"
#include "Player.hpp"

GameEngine::GameEngine(void) : _game_turn(0) {
    this->grid = Eigen::ArrayXXi::Constant(BOARD_COLS, BOARD_ROWS, state::free);
    this->_initial_timepoint = std::chrono::steady_clock::now();
    this->minmax = new MinMax(this, 3);
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
    int y = action.pos(0);
    int x = action.pos(1);

    if (action.player->get_id() == 1) {
        if (this->grid(y, x) == state::black || this->grid(y, x) == state::white || this->grid(y, x) == state::black_free) {
            return false;
        }
    }
    else {
        if (this->grid(y, x) == state::black || this->grid(y, x) == state::white || this->grid(y, x) == state::white_free) {
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
                    _check_pairs_captured(player_pairs))
                    return true;
            }
        }
    }
    return false;
}

Eigen::Array22i GameEngine::get_end_line(void) {
    Eigen::Array22i pos;
    pos << 0, 0, 0, 0;

    for (size_t col = 0; col < BOARD_ROWS; ++col) {
        for (size_t row = 0; row < BOARD_COLS; ++row) {
            if (this->grid(row, col) == state::black || this->grid(row, col) == state::white) {
                if (_check_col(col, row)) {
                    pos << col, row, col, row+4;
                    return pos;
                }
                if (_check_row(col, row)) {
                    pos << col, row, col+4, row;
                    return pos;
                }
                if (_check_dil(col, row)) {
                    pos << col, row, col-4, row+4;
                    return pos;
                }
                if (_check_dir(col, row)) {
                    pos << col, row, col+4, row+4;
                    return pos;
                }
            }
        }
    }
    return pos;
}

/* Setters */
void    GameEngine::inc_game_turn(void) {
    this->_game_turn++;
    return;
}

/* Utils */
bool    GameEngine::_check_boundary(int row, int col) {
    if (row > -1 && row < BOARD_ROWS && col > -1 && col < BOARD_COLS)
        return true;
    return false;
}

/* ********************************************************************************************************************
** UPDATE GAME STATE **************************************************************************************************
** Convention: ********************************************************************************************************
            dl1       v1       dr1
              ↖ ︎      ↑      ↗︎
                x . . x . . x
                . x . x . x .
                . . x x x . .
           h1 ← x x x o x x x → h2
                . . x x x . .
                . x . x . x .
                x . . x . . x
              ↙︎       ↓       ↘︎
            dl2       v2      dr2
******************************************************************************************************************** */

void    GameEngine::update_game_state(t_action &action, Player *player) {
    this->grid(action.pos[0], action.pos[1]) = (action.player->get_id() == 1 ? state::black : state::white);
    // TODO (alain) : detecter les db db threes et mettre 20 par exemple
    player->set_pair_captured(player->get_pair_captured() + _pair_detection(action.pos));
    _double_threes_detection();
    this->_history.push_back(action);
}

uint8_t    GameEngine::_pair_detection(Eigen::Array2i pos) {
    uint8_t    detected_pairs;

    for (int row = -1; row < 2; ++row) {
        for (int col = -1; col < 2; ++col) {
            if (_check_pair(pos, 3, row, col)) {
                this->grid((pos[0] +      row) , (pos[1] +      col))  = state::free;
                this->grid((pos[0] + (2 * row)), (pos[1] + (2 * col))) = state::free;
                detected_pairs++;
            }
        }
    }
    return detected_pairs;
}

int     GameEngine::_check_pair(Eigen::Array2i pos, int max, int row_dir, int col_dir) {
    int         row     = pos[0] + row_dir;
    int         col     = pos[1] + col_dir;
    int         cplayer = this->grid(pos[0], pos[1]);

    for (int i = 0; i < max; ++i) {
        if (_check_boundary(row, col)) {
            if (i == 0 && this->grid(row, col) != -cplayer)
                return false;
            else if (i == 1 && this->grid(row, col) != -cplayer)
                return false;
            else if (i == 2 && this->grid(row, col) != cplayer)
                return false;
            row += row_dir;
            col += col_dir;
        }
        else
            return false;
    }
    return true;
}

void    GameEngine::_double_threes_detection(void) {
    /*
        Iterate over each free, white_free and black_free cells and update the grid.
    */
    Eigen::Vector4i      count;
    Eigen::Vector4i      countblack;
    Eigen::Vector4i      countwhite;

    for (int row = 0; row < BOARD_ROWS; ++row) {
        for (int col = 0; col < BOARD_COLS; ++col) {
            if (this->grid(row, col) == state::free ||
                this->grid(row, col) == state::white_free ||
                this->grid(row, col) == state::black_free) {
                for (int p = -1; p < 2; p = p + 2) { // Switch between -1 (Player 1/black) and 1 (Player 2/white) values
                    count << 0, 0, 0, 0;
                    if (_detect_threes_xcoxox(row, col, 0, -1, p) || _detect_threes_xcxoox(row, col, 0, -1, p) ||
                        _detect_threes_xocxox(row, col, 0, -1, p) || _detect_threes_xcoox(row, col, 0, -1, p)  ||
                        _detect_threes_xocox(row, col, 0, -1, p))  // ←
                        count(0) += 1;
                    if (_detect_threes_xcoxox(row, col, 0, 1, p) || _detect_threes_xcxoox(row, col, 0, 1, p) ||
                        _detect_threes_xocxox(row, col, 0, 1, p) || _detect_threes_xcoox(row, col, 0, 1, p)) // →
                        count(0) += 1;
                    if (_detect_threes_xcoxox(row, col, -1, 0, p) || _detect_threes_xcxoox(row, col, -1, 0, p) ||
                        _detect_threes_xocxox(row, col, -1, 0, p) || _detect_threes_xcoox(row, col, -1, 0, p)  ||
                        _detect_threes_xocox(row, col, -1, 0, p)) // ↑
                        count(1) += 1;
                    if (_detect_threes_xcoxox(row, col, 1, 0, p) || _detect_threes_xcxoox(row, col, 1, 0, p) ||
                        _detect_threes_xocxox(row, col, 1, 0, p) || _detect_threes_xcoox(row, col, 1, 0, p)) // ↓
                        count(1) += 1;
                    if (_detect_threes_xcoxox(row, col, -1, -1, p) || _detect_threes_xcxoox(row, col, -1, -1, p) ||
                        _detect_threes_xocxox(row, col, -1, -1, p) || _detect_threes_xcoox(row, col, -1, -1, p)  ||
                        _detect_threes_xocox(row, col, -1, -1, p)) // ↖︎
                        count(2) += 1;
                    if (_detect_threes_xcoxox(row, col, 1, -1, p) || _detect_threes_xcxoox(row, col, 1, -1, p) ||
                        _detect_threes_xocxox(row, col, 1, -1, p) || _detect_threes_xcoox(row, col, 1, -1, p)  ||
                        _detect_threes_xocox(row, col, 1, -1, p))  // ↙︎
                        count(3) += 1;
                    if (_detect_threes_xcoxox(row, col, -1, 1, p) || _detect_threes_xcxoox(row, col, -1, 1, p) ||
                        _detect_threes_xocxox(row, col, -1, 1, p) || _detect_threes_xcoox(row, col, -1, 1, p)) // ↗︎
                        count(3) += 1;
                    if (_detect_threes_xcoxox(row, col, 1, 1, p) || _detect_threes_xcxoox(row, col, 1, 1, p) ||
                        _detect_threes_xocxox(row, col, 1, 1, p) || _detect_threes_xcoox(row, col, 1, 1, p)) // ↘︎
                        count(2) += 1;
                    if (p == -1)
                        countblack = count;
                    else
                        countwhite = count;
                }
                if (_count_double_threes(countblack) && !(_count_double_threes(countwhite))) {
                    this->grid(row, col) = state::black_free;
                }
                else if (!(_count_double_threes(countblack)) && _count_double_threes(countwhite)) {
                    this->grid(row, col) = state::white_free;
                }
                else if (_count_double_threes(countblack) && _count_double_threes(countwhite)) {
                    this->grid(row, col) = state::forbidden;
                }
                else {
                    this->grid(row, col) = state::free;
                }
            }

        }
    }
    return;
}

bool    GameEngine::_count_double_threes(Eigen::Array4i count) {
    int c_1 = 0;

    for (int i = 0; i < 4; ++i) {
        if (*(count.data() + i) == 1)
            c_1++;
    }
    if (c_1 > 1) {
        return true;
    }
    else {
        return false;
    }
}

bool    GameEngine::_detect_threes_xcoxox(int row, int col, int row_dir, int col_dir, int p) {
    // 0 x 1 0 1 0
    if (_check_boundary(row -     row_dir, col -     col_dir) &&
        _check_boundary(row + 4 * row_dir, col + 4 * col_dir)) {
        if ((this->grid(row -     row_dir, col -     col_dir) == 20 || this->grid(row -     row_dir, col -     col_dir) == -10 || this->grid(row -     row_dir, col -     col_dir) == 10 || this->grid(row -     row_dir, col -     col_dir) == 0) &&
             this->grid(row +     row_dir, col +     col_dir) == p   &&
            (this->grid(row + 2 * row_dir, col + 2 * col_dir) == 20 || this->grid(row + 2 * row_dir, col + 2 * col_dir) == -10 || this->grid(row + 2 * row_dir, col + 2 * col_dir) == 10 || this->grid(row + 2 * row_dir, col + 2 * col_dir) == 0) &&
             this->grid(row + 3 * row_dir, col + 3 * col_dir) == p   &&
            (this->grid(row + 4 * row_dir, col + 4 * col_dir) == 20 || this->grid(row + 4 * row_dir, col + 4 * col_dir) == -10 || this->grid(row + 4 * row_dir, col + 4 * col_dir) == 10 || this->grid(row + 4 * row_dir, col + 4 * col_dir) == 0)) {
            return true;
        }
    }
    return false;
}

bool    GameEngine::_detect_threes_xcxoox(int row, int col, int row_dir, int col_dir, int p) {
    // 0 x 0 1 1 0
    if(_check_boundary(row -     row_dir, col -     col_dir) &&
       _check_boundary(row + 4 * row_dir, col + 4 * col_dir)) {
        if ((this->grid(row -     row_dir, col -     col_dir) == 20 || this->grid(row -     row_dir, col -     col_dir) == -10 || this->grid(row -     row_dir, col -     col_dir) == 10 || this->grid(row -     row_dir, col -     col_dir) == 0) &&
            (this->grid(row +     row_dir, col +     col_dir) == 20 || this->grid(row +     row_dir, col +     col_dir) == -10 || this->grid(row +     row_dir, col +     col_dir) == 10 || this->grid(row +     row_dir, col +     col_dir) == 0) &&
             this->grid(row + 2 * row_dir, col + 2 * col_dir) == p &&
             this->grid(row + 3 * row_dir, col + 3 * col_dir) == p &&
            (this->grid(row + 4 * row_dir, col + 4 * col_dir) == 20 || this->grid(row + 4 * row_dir, col + 4 * col_dir) == -10 || this->grid(row + 4 * row_dir, col + 4 * col_dir) == 10 || this->grid(row + 4 * row_dir, col + 4 * col_dir) == 0)) {
            return true;
        }
    }
    return false;
}

bool    GameEngine::_detect_threes_xocxox(int row, int col, int row_dir, int col_dir, int p) {
    // 0 1 x 0 1 0
    if(_check_boundary(row - 2 * row_dir, col - 2 * col_dir) &&
       _check_boundary(row + 3 * row_dir, col + 3 * col_dir)) {
        if ((this->grid(row - 2 * row_dir, col - 2 * col_dir) == 20 || this->grid(row - 2 * row_dir, col - 2 * col_dir) == -10 || this->grid(row - 2 * row_dir, col - 2 * col_dir) == 10 || this->grid(row - 2 * row_dir, col - 2 * col_dir) == 0) &&
             this->grid(row -     row_dir, col -     col_dir) == p &&
            (this->grid(row +     row_dir, col +     col_dir) == 20 || this->grid(row +     row_dir, col +     col_dir) == -10 || this->grid(row +     row_dir, col +     col_dir) == 10 || this->grid(row +     row_dir, col +     col_dir) == 0) &&
             this->grid(row + 2 * row_dir, col + 2 * col_dir) == p &&
            (this->grid(row + 3 * row_dir, col + 3 * col_dir) == 20 || this->grid(row + 3 * row_dir, col + 3 * col_dir) == -10 || this->grid(row + 3 * row_dir, col + 3 * col_dir) == 10 || this->grid(row + 3 * row_dir, col + 3 * col_dir) == 0)) {
            return true;
        }
    }
    return false;
}

bool    GameEngine::_detect_threes_xcoox(int row, int col, int row_dir, int col_dir, int p) {
    // 0 x 1 1 0
    if (_check_boundary(row + -1 * row_dir, col + -1 * col_dir) &&
        _check_boundary(row + 3 * row_dir, col + 3 * col_dir)) {
        if ((this->grid(row - 1 * row_dir, col - 1 * col_dir) == 20 || this->grid(row - 1 * row_dir, col - 1 * col_dir) == -10 || this->grid(row - 1 * row_dir, col - 1 * col_dir) == 10 || this->grid(row - 1 * row_dir, col - 1 * col_dir) == 0) &&
             this->grid(row +     row_dir, col +     col_dir) == p &&
             this->grid(row + 2 * row_dir, col + 2 * col_dir) == p &&
            (this->grid(row + 3 * row_dir, col + 3 * col_dir) == 20 || this->grid(row + 3 * row_dir, col + 3 * col_dir) == -10 || this->grid(row + 3 * row_dir, col + 3 * col_dir) == 10 || this->grid(row + 3 * row_dir, col + 3 * col_dir) == 0)) {
            return true;
        }
    }
    return false;
}

bool    GameEngine::_detect_threes_xocox(int row, int col, int row_dir, int col_dir, int p) {
    // 0 1 x 1 0
    if (_check_boundary(row + 2 * row_dir, col + 2 * col_dir) &&
        _check_boundary(row - 2 * row_dir, col - 2 * col_dir)) {
        if ((this->grid(row + 2 * row_dir, col + 2 * col_dir) == 20 || this->grid(row + 2 * row_dir, col + 2 * col_dir) == -10 || this->grid(row + 2 * row_dir, col + 2 * col_dir) == 10 || this->grid(row + 2 * row_dir, col + 2 * col_dir) == 0) &&
             this->grid(row +     row_dir, col +     col_dir) == p &&
             this->grid(row - 1 * row_dir, col - 1 * col_dir) == p &&
            (this->grid(row - 2 * row_dir, col - 2 * col_dir) == 20 || this->grid(row - 2 * row_dir, col - 2 * col_dir) == -10 || this->grid(row - 2 * row_dir, col - 2 * col_dir) == 10 || this->grid(row - 2 * row_dir, col - 2 * col_dir) == 0)) {
            return true;
        }
    }
    return false;
}

void    GameEngine::delete_last_action(void) {
    t_action    last;

    if (this->_history.size() > 0) {
        last = this->_history.back();
        this->grid = last.old_grid;
        this->grid(last.pos[0], last.pos[1]) = (last.player->get_id() == 1 ? state::black : state::white);
        last.player->set_pair_captured(last.player->get_pair_captured() - _pair_detection(last.pos));
        this->grid = last.old_grid;
        this->_history.pop_back();
    }
}

/*
int     GameEngine::_sum_free_threes(int row, int col, int max, int row_dir, int col_dir) {
    int         sum = 0;

    for (int i = max; i > 0; --i) {
        if (row > -1 && row < 19 && col > -1 && col < 19){
            sum += this->grid(row, col);
            row += row_dir;
            col += col_dir;
        }
        else
            break;
    }
    return sum;
}
*/

/* ********************************************************************************************************************
** CHECK END **********************************************************************************************************
******************************************************************************************************************** */

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
bool    GameEngine::_check_pairs_captured(uint8_t pairs) {
    if (pairs != 5)
        return false;
    return true;
}
