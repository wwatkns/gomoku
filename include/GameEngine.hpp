#ifndef GAMEENGINE_HPP
# define GAMEENGINE_HPP

# include <iostream>
// # include <ctime>
# include <chrono>
# include <list>
# include <Eigen/Dense>

# define BOARD_COLS 19
# define BOARD_ROWS 19


typedef struct  s_action {
    Eigen::Array2i                  pos;
    unsigned short                  player_id;
    unsigned long                   id;
    std::chrono::duration<double>   timepoint;
}               t_action;


class GameEngine {

public:
    GameEngine(void);
    GameEngine(GameEngine const &src);
    ~GameEngine(void);
    GameEngine	&operator=(GameEngine const &rhs);

    bool                check_action(t_action &action); // check if action is valid
    bool                check_end(void);
    void                update_game_state(t_action &action); // update the game state given an action

    /* Getters */
    Eigen::ArrayXXi                         get_grid(void) const;
    std::list<t_action>                     get_history(void) const;
    unsigned long                           get_history_size(void) const;
    std::chrono::steady_clock::time_point   get_initial_timepoint(void) const;
    /* Setters */
    void                                    set_grid(Eigen::ArrayXXi grid);

private:
    Eigen::ArrayXXi                         _grid;
    std::list<t_action>                     _history;
    std::chrono::steady_clock::time_point   _initial_timepoint;
    /* the possible states of the board cells */
    struct state { enum { free, black, white, black_free, white_free }; };

};

#endif
