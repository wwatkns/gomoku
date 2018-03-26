#ifndef COMPUTER_HPP
# define COMPUTER_HPP

# include "Player.hpp"
// # include "AIPlayer.hpp"
# include "AIAlgorithms.hpp"

class Computer : public Player {

public:
    Computer(GameEngine *game_engine, uint8_t id, int algo_type, int depth);
    Computer(Computer const &src);
    ~Computer(void);
    Computer	&operator=(Computer const &rhs);

    virtual bool    play(Player *other);
    AIPlayer        *ai_algorithm;
    int             depth;
};

#endif
