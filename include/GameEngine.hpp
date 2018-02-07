#ifndef GAMEENGINE_HPP
# define GAMEENGINE_HPP

# include <vector>
# include <Eigen/Dense>
// # include "Player.hpp"

# define BOARD_COLS 19
# define BOARD_ROWS 19


typedef struct  s_action { // Redefinition, TODO, delete redondant code
    Eigen::Array2i      pos;
    unsigned short      player_index;
    unsigned long       num;
    unsigned long       timestamp;
}               t_action;


class GameEngine {

public:
    GameEngine(void);
    GameEngine(GameEngine const &src);
    ~GameEngine(void);
    GameEngine	&operator=(GameEngine const &rhs);

    bool            check_action(t_action &action); // check if action is valid
    bool            check_end(void);
    void            update_game_state(t_action &action); // update the game state given an action

    /* Getters */
    Eigen::ArrayXXi get_grid(void) const;
    /* Setters */
    void            set_grid(Eigen::ArrayXXi grid);

private:
    Eigen::ArrayXXi         _grid;
    std::vector<t_action>   _history;
    /* the possible states of the board cells */
    struct state { enum { free, black, white, black_free, white_free }; };

};

#endif
