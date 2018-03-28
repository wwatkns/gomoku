#ifndef HUMAN_HPP
# define HUMAN_HPP

# include "Player.hpp"

class Human : public Player {

public:
    Human(GameEngine *game_engine, GraphicalInterface *gui, uint8_t id, int algo_type, int depth);
    Human(Human const &src);
    ~Human(void);
    Human	&operator=(Human const &rhs);

    virtual bool    play(Player *other);

    // AlphaBetaWithIterativeDeepening get_alphaBeta(void) const { return (_alphaBeta); };

private:
    std::chrono::duration<double, std::milli>   _action_duration;
    // AlphaBetaWithIterativeDeepening             _alphaBeta;
};

#endif
