#ifndef PLAYER_HPP
# define PLAYER_HPP

# include <Eigen/Dense>
# include "GraphicalInterface.hpp"
# include "BitBoard.hpp"
# include "AIAlgorithms.hpp"

class           GameEngine;
typedef struct  s_action t_action;

class Player {

public:
    Player(GameEngine *game_engine, GraphicalInterface *gui, uint8_t id, int algo_type, int depth);
    Player(Player const &src);
    virtual ~Player() {};
    Player	&operator=(Player const &rhs);

    virtual bool    play(Player *other) = 0;

    /* Getters */
    GameEngine      *get_game_engine(void) const { return _game_engine; };
    uint8_t         get_id(void) const { return _id; };
    uint8_t         get_pairs_captured(void) const { return _pairs_captured; };
    AIPlayer        *get_ai_algorithm(void) const { return (_ai_algorithm); };
    /* Setters */
    void            set_pairs_captured(uint8_t pairs) { _pairs_captured = pairs; };

    uint8_t         type;
    BitBoard        board;
    BitBoard        board_forbidden;
    Eigen::Array2i  suggested_move;
    int64_t         current_score;

protected:
    GameEngine          *_game_engine;
    GraphicalInterface  *_gui;
    AIPlayer            *_ai_algorithm;
    uint8_t             _id;
    uint8_t             _pairs_captured;
};

#endif
