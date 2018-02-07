#ifndef PLAYER_HPP
# define PLAYER_HPP

# include <Eigen/Dense>
# include "GameEngine.hpp"

class Player; // define the Player class before the definition so that the struct knows what's up

// typedef struct  s_action {
//     Eigen::Array2i      pos;
//     unsigned short      player_index;
//     unsigned long       num;
//     unsigned long       timestamp;
// }               t_action;


class Player {

public:
    Player(GameEngine *game_engine, unsigned short index);
    Player(Player const &src);
    ~Player(void);
    Player	&operator=(Player const &rhs);

    virtual t_action    play(void) = 0;

    /* Getters */
    GameEngine      *get_game_engine(void) const;
    unsigned short  get_index(void) const;

protected:
    GameEngine      *_game_engine;
    unsigned short  _index;

};

#endif
