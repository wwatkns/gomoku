#ifndef HUMAN_HPP
# define HUMAN_HPP

# include "Player.hpp"

class Human : public Player {

public:
    Human(GameEngine *game_engine, unsigned short index);
    Human(Human const &src);
    ~Human(void);
    Human	&operator=(Human const &rhs);

    virtual t_action    play(void);
};

#endif
