#ifndef GAMEENGINE_HPP
# define GAMEENGINE_HPP

# include <ctime>
# include <list>
# include <Eigen/Dense>

# define BOARD_COLS 19
# define BOARD_ROWS 19


typedef struct  s_action {
    Eigen::Array2i      pos;
    unsigned short      player_id;
    unsigned long       num;
    std::time_t         timestamp;
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
    std::list<t_action>     _history;
    std::time_t             _initial_timestamp;
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

};

#endif
