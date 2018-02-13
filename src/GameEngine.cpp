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
        if (this->grid(y, x) == -1 || this->grid(y, x) == 1 || this->grid(y, x) == -10) {
            return false;
        }
    }
    else {
        if (this->grid(y, x) == -1 || this->grid(y, x) == 1 || this->grid(y, x) == 10) {
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

/*
    Update Game state
*/

bool    GameEngine::_check_boundary(int row, int col) {
    if (row > -1 && row < BOARD_ROWS && col > -1 && col < BOARD_COLS)
        return true;
    return false;
}

int     GameEngine::_check_pair(Eigen::Array2i pos, int max, int row_dir, int col_dir) {
    /*

                   row - 1
                      ↑
                x . . x . . x
                . x . x . x .
                . . x x x . .
     col - 1  ← x x x o x x x → col + 1
                . . x x x . .
                . x . x . x .
                x . . x . . x
                      ↓
                   row + 1

    */
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

/*
itere sur tous les 0:
    sommes = calculer pour chacune de ces coordonnees la somme des lignes dans directions differentes
        si somme == 2 ou == -2
            compteur += 1
        si compteur == 2
            mettre un 10 ou -10 selon autorisation et passer à coord suivante

*/
void    GameEngine::_double_threes_detection(void) {
    /*
    !!! Voir si accéléré avec .data() plutot
    */
    // int size = BOARD_COLS * BOARD_ROWS;
    // for (int i = 0; i < size; ++i) {
        // if (*(this->grid.data()) == 0) {
            // Sum the 6 directions
            // TODO comment detecter les directions
            // sum row:
            // sum_h1 = *(this->grid.data() +     BOARD_ROWS) + *(this->grid.data() + 2 * BOARD_ROWS) +
                    //  *(this->grid.data() + 3 * BOARD_ROWS) // Not yet finished
        // }
    // }
    int count_p1;
    int count_p2;

    for (int row = 0; row < BOARD_ROWS; ++row) {
        for (int col = 0; col < BOARD_COLS; ++col) {
            count_p1 = 0;
            count_p2 = 0;
            if (this->grid(row, col) == 0) {
                // h1
                if (_sum_free_threes(row, col, 3, 0, -1) == -2)
                    count_p1 += -1;
                if (_sum_free_threes(row, col, 3, 0, -1) == 2)
                    count_p2 += 1;
                // h2
                if (_sum_free_threes(row, col, 3, 0, 1) == -2)
                    count_p1 += -1;
                if (_sum_free_threes(row, col, 3, 0, 1) == 2)
                    count_p2 += 1;
                // v1
                if (_sum_free_threes(row, col, 3, -1, 0) == -2)
                    count_p1 += -10;
                if (_sum_free_threes(row, col, 3, -1, 0) == 2)
                    count_p2 += 10;
                // v2
                if (_sum_free_threes(row, col, 3, 1, 0) == -2)
                    count_p1 += -10;
                if (_sum_free_threes(row, col, 3, 1, 0) == 2)
                    count_p2 += 10;
                // dl1
                if (_sum_free_threes(row, col, 3, -1, -1) == -2)
                    count_p1 += -100;
                if (_sum_free_threes(row, col, 3, -1, -1) == 2)
                    count_p2 += 100;
                // dl2
                if (_sum_free_threes(row, col, 3, 1, -1) == -2)
                    count_p1 += -100;
                if (_sum_free_threes(row, col, 3, 1, -1) == 2)
                    count_p2 += 100;
                // dr1
                if (_sum_free_threes(row, col, 3, -1, 1) == -2)
                    count_p1 += -1000;
                if (_sum_free_threes(row, col, 3, -1, 1) == 2)
                    count_p2 += 1000;
                // dr2
                if (_sum_free_threes(row, col, 3, 1, 1) == -2)
                    count_p1 += -1000;
                if (_sum_free_threes(row, col, 3, 1, 1) == 2)
                    count_p2 += 1000;
                std::cout << "count_p1: " << count_p1 << std::endl;
                std::cout << "count_p2: " << count_p2 << std::endl;
                if (count_p1 % 2 == 1 || count_p1 % 20 == 10 || count_p1 % 200 == 100 || count_p1 % 2000 == 1000) {
                    this->grid(row, col) = -10;
                }
                if (count_p2 % 2 == 1 || count_p2 % 20 == 10 || count_p2 % 200 == 100 || count_p2 % 2000 == 1000) {
                    this->grid(row, col) = 10;
                }
            }
        }
        std::cout << this->grid << std::endl;
    }
    return;
}

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

void    GameEngine::update_game_state(t_action &action, Player &player) {
    this->grid(action.pos[0], action.pos[1]) = (action.player_id == 1 ? state::black : state::white);
    // TODO (alain) : detecter les doubles threes et mettre 10/-10 aux emplacements 
    _double_threes_detection();
    _pair_detection(action.pos, player);
    this->_history.push_back(action);
}

/*
    End of Update Game State
*/

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

/* Setters */
void    GameEngine::inc_game_turn(void) {
    this->_game_turn++;
    return;
}
