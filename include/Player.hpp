#ifndef PLAYER_HPP
# define PLAYER_HPP

# include <Eigen/Dense>
# include "GameEngine.hpp"
# include "GraphicalInterface.hpp"

class           GameEngine;
typedef struct  s_action t_action;

class Player {

public:
    Player(GameEngine *game_engine, GraphicalInterface *gui, unsigned short id);
    Player(Player const &src);
    virtual ~Player() {};
    Player	&operator=(Player const &rhs);

    virtual t_action    play(void) = 0;

    /* Getters */
    GameEngine      *get_game_engine(void) const;
    unsigned short  get_id(void) const;
    unsigned short  get_pair_captured(void) const;

protected:
    GameEngine          *_game_engine;
    GraphicalInterface  *_gui;
    uint8_t             _id;
    uint8_t             _pair_captured;

};

#endif
