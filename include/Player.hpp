#ifndef PLAYER_HPP
# define PLAYER_HPP

# include <Eigen/Dense>
# include "GameEngine.hpp"

class Player {

public:
    Player(GameEngine *game_engine, unsigned short id);
    Player(Player const &src);
    virtual ~Player() {};
    Player	&operator=(Player const &rhs);

    virtual t_action    play(void) = 0;

    /* Getters */
    GameEngine      *get_game_engine(void) const;
    unsigned short  get_id(void) const;

protected:
    GameEngine      *_game_engine;
    uint8_t         _id;

};

#endif
