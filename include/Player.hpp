#ifndef PLAYER_HPP
# define PLAYER_HPP

# include <Eigen/Dense>
# include "GameEngine.hpp"

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
