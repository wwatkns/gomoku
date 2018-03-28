#include "Human.hpp"

Human::Human(GameEngine *game_engine, GraphicalInterface *gui, uint8_t id, int algo_type, int depth) : Player(game_engine, gui, id, algo_type, depth) {
    this->_action_duration = std::chrono::steady_clock::duration::zero();
    this->type = 0;
}

Human::Human(Human const &src) : Player(src) {
    *this = src;
}

Human::~Human(void) {
    delete this->_ai_algorithm;
}

Human	&Human::operator=(Human const &src) {
    this->_game_engine = src.get_game_engine();
    this->_id = src.get_id();
    this->_ai_algorithm = src.get_ai_algorithm();
    return (*this);
}

bool    Human::play(Player *other) {
    t_action    action;

    if (this->_gui->get_sg() && this->suggested_move(0) == -1) {
        t_node  root = create_node(*this, *other);
        t_ret ret = (*this->_ai_algorithm)(root);
        this->suggested_move = { range(ret.p / 19, 0, 18), range(ret.p % 19, 0, 18) };
    }
    if (this->_action_duration == std::chrono::steady_clock::duration::zero())
        this->_action_duration = this->_gui->get_analytics()->get_chronometer()->get_elapsed();

    if (this->_gui->get_mouse_action() && this->_gui->check_mouse_on_board() && this->_gui->check_pause() == false) {
        action.timepoint = std::chrono::steady_clock::now() - this->_game_engine->get_initial_timepoint();
        action.duration = (this->_gui->get_analytics()->get_chronometer()->get_elapsed() - this->_action_duration);
        action.pos = this->_gui->screen_to_grid(this->_gui->get_mouse_pos());
        action.id = this->_game_engine->get_history_size() + 1;
        action.p1_last = this->board;
        action.p2_last = other->board;
        action.pid = this->_id;
        action.ppc = this->_pairs_captured;
        if (this->_game_engine->check_action(action, *this, *other)) {
            this->_game_engine->update_game_state(action, this, other);
            this->suggested_move = { -1, -1 };
            this->_action_duration = std::chrono::steady_clock::duration::zero();
            return (true);
        }
    }
    return (false);
}
