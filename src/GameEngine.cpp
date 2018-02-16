#include "GameEngine.hpp"
#include "Player.hpp"

GameEngine::GameEngine(void) : _game_turn(0) {
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
    int y = action.pos(0);
    int x = action.pos(1);

    if (action.player_id == 1) {
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

void    GameEngine::update_game_state(t_action &action, Player &player) {
    this->grid(action.pos[0], action.pos[1]) = (action.player_id == 1 ? state::black : state::white);
    // TODO (alain) : detecter les doubles threes et mettre 10/-10 aux emplacements 
    // _double_threes_detection(action.pos);
    _double_threes_detection();
    _pair_detection(action.pos, player);
    this->_history.push_back(action);
}

void    GameEngine::_pair_detection(Eigen::Array2i pos, Player &player) {
    for (int row = -1; row < 2; ++row) {
        for (int col = -1; col < 2; ++col) {
            if (_check_pair(pos, 3, row, col)) {
                this->grid((pos[0] +      row) , (pos[1] +      col))  = state::free;
                this->grid((pos[0] + (2 * row)), (pos[1] + (2 * col))) = state::free;
                player.inc_pair_captured();
            }
        }
    }
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
    Eigen::Array4i      count;

    for (int row = 0; row < BOARD_ROWS; ++row) {
        for (int col = 0; col < BOARD_COLS; ++col) {
            for (int p = -1; p < 2; p = p + 2) { // Switch between -1 (Player 1) and 1 (Player 2) values
                count << 0, 0, 0, 0;
                if (this->grid(row, col) == state::free ||
                    this->grid(row, col) == state::white_free ||
                    this->grid(row, col) == state::black_free) {
                    if (_detect_threes(row, col, 0, -1, p))  // ←
                        count(0) += 1;
                    if (_detect_threes(row, col, 0, 1, p))   // →
                        count(0) += 1;
                    if (_detect_threes(row, col, -1, 0, p))  // ↑
                        count(1) += 1;
                    if (_detect_threes(row, col, 1, 0, p))   // ↓
                        count(1) += 1;
                    if (_detect_threes(row, col, -1, -1, p)) // ↖︎
                        count(2) += 1;
                    if (_detect_threes(row, col, 1, -1, p))  // ↙︎
                        count(2) += 1;
                    if (_detect_threes(row, col, -1, 1, p))  // ↗︎
                        count(3) += 1;
                    if (_detect_threes(row, col, 1, 1, p))   // ↘︎
                        count(3) += 1;

                    if (row == 2 && col == 2)                    
                        std::cout << "count: " << count << std::endl; // delete me

                    if (this->grid(row, col) == state::free) {
                        if (_count_double_threes(count) && p == -1) {
                            this->grid(row, col) = state::black_free;
                        }
                        if (_count_double_threes(count) && p == 1) {
                            this->grid(row, col) = state::white_free;
                        }
                    }
                    else if (this->grid(row, col) == state::black_free ||
                             this->grid(row, col) == state::white_free) {
                        if (!_count_double_threes(count))
                            this->grid(row, col) = state::free;
                    }
                }
            }
        }
    }
    std::cout << this->grid << std::endl << std::endl; // delete me
    return;
}

bool    GameEngine::_count_double_threes(Eigen::Array4i count) {
    int c_0 = 0;
    int c_1 = 0;
    int c_2 = 0;

    // std::cout << "### _count_double_threes" << std::endl; // delete me

    for (int i = 0; i < 4; ++i) {
        if (*(count.data() + i) == 0)
            c_0++;
        else if (*(count.data() + i) == 1)
            c_1++;
        else if (*(count.data() + i) == 2)
            c_2++;
    }
    if (c_1 > 1) {
        std::cout << "### _count_double_threes ## END # true" << std::endl; // delete me
        return true;
    }
    else {
        std::cout << "### _count_double_threes ## END # false" << std::endl; // delete me
        return false;
    }
}

bool    GameEngine::_detect_threes(int row, int col, int row_dir, int col_dir, int p) {
    // Detect specific cases undetected by _detect_threes, example:
    // . . . . .    . . . . . .
    // . . o . .    . . o . . . 
    // . . o . . or . o x . o .
    // . o x o .    . . o . . .
    // . . . . .    . . . . . . 

    // 0 x 1 0 1 0
    if (_check_boundary(row -     row_dir, col -     col_dir) &&
        _check_boundary(row + 4 * row_dir, col + 4 * col_dir)) {
        if (this->grid(row -     row_dir, col -     col_dir) == 0 && // 0 x
            this->grid(row +     row_dir, col +     col_dir) == p && // 1
            this->grid(row + 2 * row_dir, col + 2 * col_dir) == 0 && // 0
            this->grid(row + 3 * row_dir, col + 3 * col_dir) == p && // 1
            this->grid(row + 4 * row_dir, col + 4 * col_dir) == 0) { // 0
            // std::cout << "detected @ row: " << row << " col: " << col << " | form 0 x 1 0 1 0" << std::endl; // delete me
            return true;
        }
    }
    // 0 x 0 1 1 0
    if(_check_boundary(row -     row_dir, col -     col_dir) &&
       _check_boundary(row + 4 * row_dir, col + 4 * col_dir)) {
        if (this->grid(row -     row_dir, col -     col_dir) == 0 && // 0 x
            this->grid(row +     row_dir, col +     col_dir) == 0 && // 0
            this->grid(row + 2 * row_dir, col + 2 * col_dir) == p && // 1
            this->grid(row + 3 * row_dir, col + 3 * col_dir) == p && // 1
            this->grid(row + 4 * row_dir, col + 4 * col_dir) == 0) { // 0
            // std::cout << "detected @ row: " << row << " col: " << col << " | form 0 x 0 1 1 0" << std::endl; // delete me
            return true;
        }
    }
    // 0 1 x 0 1 0
    if(_check_boundary(row - 2 * row_dir, col - 2 * col_dir) &&
       _check_boundary(row + 3 * row_dir, col + 3 * col_dir)) {
        if (this->grid(row - 2 * row_dir, col - 2 * col_dir) == 0 && // 0
            this->grid(row -     row_dir, col -     col_dir) == p && // 1 x
            this->grid(row +     row_dir, col +     col_dir) == 0 && // 0
            this->grid(row + 2 * row_dir, col + 2 * col_dir) == p && // 1
            this->grid(row + 3 * row_dir, col + 3 * col_dir) == 0) { // 0
            // std::cout << "detected @ row: " << row << " col: " << col << " | form 0 1 x 0 1 0" << std::endl; // delete me
            return true;
        }
    }
    // 0 1 0 x 1 0
    if(_check_boundary(row - 3 * row_dir, col - 3 * col_dir) &&
       _check_boundary(row + 2 * row_dir, col + 2 * col_dir)) {
        if (this->grid(row - 3 * row_dir, col - 3 * col_dir) == 0 && // 0
            this->grid(row - 2 * row_dir, col - 2 * col_dir) == p && // 1
            this->grid(row -     row_dir, col -     col_dir) == 0 && // 0 x
            this->grid(row +     row_dir, col +     col_dir) == p && // 1
            this->grid(row + 2 * row_dir, col + 2 * col_dir) == 0) { // 0
            // std::cout << "detected @ row: " << row << " col: " << col << " | form 0 1 0 x 1 0" << std::endl; // delete me            
            return true;
        }
    }
    // 0 x 1 1 0
    if (_check_boundary(row + -1 * row_dir, col + -1 * col_dir) &&
        _check_boundary(row + 3 * row_dir, col + 3 * col_dir)) {
        if (this->grid(row - 1 * row_dir, col - 1 * col_dir) == 0 && // 0 x
            this->grid(row +     row_dir, col +     col_dir) == p && // 1
            this->grid(row + 2 * row_dir, col + 2 * col_dir) == p && // 1
            this->grid(row + 3 * row_dir, col + 3 * col_dir) == 0) { // 0
            // std::cout << "detected @ row: " << row << " col: " << col << " | form 0 x 1 1 0" << std::endl; // delete me            
            return true;
        }
    }
    // 0 1 x 1 0
    if (_check_boundary(row + 2 * row_dir, col + 2 * col_dir) &&
        _check_boundary(row - 2 * row_dir, col - 2 * col_dir)) {
        if (this->grid(row + 2 * row_dir, col + 2 * col_dir) == 0 && // 0
            this->grid(row +     row_dir, col +     col_dir) == p && // 1 x
            this->grid(row - 1 * row_dir, col - 1 * col_dir) == p && // 1
            this->grid(row - 2 * row_dir, col - 2 * col_dir) == 0) { // 0
            if (row == 2 && col == 2)
                std::cout << "detected @ row: " << row << " col: " << col << " | form 0 1 x 1 0" << std::endl; // delete me            
            return true;
        }
    }
    return false;
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
