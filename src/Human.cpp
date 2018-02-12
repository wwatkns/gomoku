#include "Human.hpp"

Human::Human(GameEngine *game_engine, GraphicalInterface *gui, unsigned short id) : Player(game_engine, gui, id) {
}

Human::Human(Human const &src) : Player(src) {
    *this = src;
}

Human::~Human(void) {
}

Human	&Human::operator=(Human const &src) {
    this->_game_engine = src.get_game_engine();
    this->_id = src.get_id();
    return (*this);
}

t_action    Human::play(void) {
    t_action    action;

    while (true) {
        this->_gui->update_events();
        if (this->_gui->check_mouse_action()) {
            action.pos = this->_gui->screen_to_grid(this->_gui->get_mouse_pos());
            action.player_id = this->_id;
            action.id = this->_game_engine->get_history_size() + 1;
            action.timepoint = std::chrono::steady_clock::now() - this->_game_engine->get_initial_timepoint();
            if (this->_game_engine->check_action(action))
                break;
        }
        if (this->_gui->check_close())
            break;
        this->_gui->update_display();
    }
    return action;
}
