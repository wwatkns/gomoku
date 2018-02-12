#ifndef PLAYER_HPP
# define PLAYER_HPP

# include <Eigen/Dense>
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
    GameEngine      *get_game_engine(void) const { return _game_engine; };
    uint8_t         get_id(void) const { return _id; };
    uint8_t         get_pair_captured(void) const { return _pair_captured; };

protected:
    GameEngine          *_game_engine;
    GraphicalInterface  *_gui;
    uint8_t             _id;
    uint8_t             _pair_captured;

};

#endif
