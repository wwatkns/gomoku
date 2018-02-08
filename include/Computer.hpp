#ifndef COMPUTER_HPP
# define COMPUTER_HPP

# include "Player.hpp"

class Computer : public Player {

public:
    Computer(GameEngine *game_engine, unsigned short id);
    Computer(Computer const &src);
    ~Computer(void);
    Computer	&operator=(Computer const &rhs);

    virtual t_action    play(void);
};

#endif
