#ifndef COMPUTER_HPP
# define COMPUTER_HPP

# include "Player.hpp"

class Computer : public Player {

public:
    Computer(GameEngine *game_engine, GraphicalInterface *gui, uint8_t id);
    Computer(Computer const &src);
    ~Computer(void);
    Computer	&operator=(Computer const &rhs);

    virtual bool    play(Player *other);

    AlphaBeta       get_alphaBeta(void) const { return (_alphaBeta); };

private:
    AlphaBeta   _alphaBeta;
};

#endif
