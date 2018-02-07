#include "Human.hpp"

Human::Human(GameEngine *game_engine, unsigned short index) : Player(game_engine, index) {
}

Human::Human(Human const &src) : Player(src) {
    *this = src;
}

Human::~Human(void) {
}

Human	&Human::operator=(Human const &src) {
    this->_game_engine = src.get_game_engine();
    this->_index = src.get_index();
    return (*this);
}

t_action    Human::play(void) {
    t_action    action;

    /*  Await the players input, check if those are valid, if not,
        loop again.
    */
    return action;
}
