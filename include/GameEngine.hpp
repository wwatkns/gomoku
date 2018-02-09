#ifndef GAMEENGINE_HPP
# define GAMEENGINE_HPP

# include <iostream>
# include <chrono>
# include <list>
# include <Eigen/Dense>

# include "Player.hpp"

# define BOARD_COLS 19
# define BOARD_ROWS 19
# define ALIGNTOWIN 5


typedef struct  s_action {
    Eigen::Array2i                  pos;
    uint8_t                         player_id;
    uint64_t                        id;
    std::chrono::duration<double>   timepoint;
}               t_action;

class Player;

class GameEngine {

public:
    GameEngine(void);
    GameEngine(GameEngine const &src);
    ~GameEngine(void);
    GameEngine	&operator=(GameEngine const &rhs);

    bool                check_action(t_action &action); // check if action is valid
    bool                check_end(Player *player);
    void                update_game_state(t_action &action); // update the game state given an action

    /* Getters */
    std::list<t_action>                     get_history(void) const;
    uint64_t                                get_history_size(void) const;
    std::chrono::steady_clock::time_point   get_initial_timepoint(void) const;
    /* Setters */

    Eigen::ArrayXXi                         grid;

private:
    std::list<t_action>                     _history;
    std::chrono::steady_clock::time_point   _initial_timepoint;
    /* the possible states of the board cells */
    struct state {
        enum {
            black_free = -10,
            black = -1,
            free = 0,
            white = 1,
            white_free = 10 
        }; 
    };

    bool            _check_col(size_t col, size_t row);
    bool            _check_row(size_t col, size_t row);
    bool            _check_dil(size_t col, size_t row);
    bool            _check_dir(size_t col, size_t row);
    bool            _check_pairs(Player *player);

};

#endif
