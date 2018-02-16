#ifndef HUMAN_HPP
# define HUMAN_HPP

# include "Player.hpp"

class Human : public Player {

public:
    Human(GameEngine *game_engine, GraphicalInterface *gui, uint8_t id);
    Human(Human const &src);
    ~Human(void);
    Human	&operator=(Human const &rhs);

    virtual void    play(void);
};

#endif
