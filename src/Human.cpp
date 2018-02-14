#include "Human.hpp"

Human::Human(GameEngine *game_engine, GraphicalInterface *gui, uint8_t id) : Player(game_engine, gui, id) {
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

void    Human::play(void) {
    t_action                                action;
    std::chrono::steady_clock::time_point   action_beg = std::chrono::steady_clock::now();

    while (true) {
        this->_gui->update_events();
        if (this->_gui->get_mouse_action() && this->_gui->check_mouse_on_board()) {
            action.pos = this->_gui->screen_to_grid(this->_gui->get_mouse_pos());
            action.player_id = this->_id;
            action.id = this->_game_engine->get_history_size() + 1;
            action.old_grid = this->_game_engine->grid;
            action.timepoint = std::chrono::steady_clock::now() - this->_game_engine->get_initial_timepoint();
            action.duration = std::chrono::steady_clock::now() - action_beg;
            if (this->_game_engine->check_action(action)) {
                this->_game_engine->update_game_state(action, this);
                break;
            }
        }
        if (this->_gui->check_close() || this->_gui->check_undo())
            break;
        this->_gui->update_display();
    }
}
