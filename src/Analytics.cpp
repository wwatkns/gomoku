#include "Analytics.hpp"
#include "Player.hpp"

Analytics::Analytics(GameEngine *game_engine, FontHandler *font_handler, float res_ratio) : _game_engine(game_engine), _font_handler(font_handler), _res_ratio(res_ratio), _player_1(nullptr), _player_2(nullptr) {
    this->_update_analytics(true);
    for (std::map<std::string,s_data>::iterator it=this->_data.begin(); it != this->_data.end(); it++) {
        this->_font_handler->create_text(&it->second.text, it->second.pos);
    }
}

Analytics::Analytics(Analytics const &src) {
    *this = src;
}

Analytics::~Analytics(void) {
}

Analytics	&Analytics::operator=(Analytics const &src) {
    this->_game_engine = src.get_game_engine();
    this->_font_handler = src.get_font_handler();
    return (*this);
}

static std::string  fzeros(std::string str, uint32_t width) {
    if (str.size() < width)
        return std::string(width - str.size(), '0') + str;
    return str;
}

static std::string  format_time_since_start(std::chrono::duration<double, std::milli> elapsed) {
    std::string formated_time;
    uint32_t    ms;

    ms = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
    formated_time = fzeros(std::to_string(ms/60000), 2)+":"+fzeros(std::to_string(ms%60000/1000), 2)+"."+fzeros(std::to_string(ms%1000/10), 2)+" min";
    return formated_time;
}

static std::string  format_duration(uint32_t ms) {
    std::string formated_time;

    formated_time = fzeros(std::to_string(ms/60000), 2)+":"+fzeros(std::to_string(ms%60000/1000), 2)+"."+fzeros(std::to_string(ms%1000), 4)+" ms";
    return formated_time;
}

static uint32_t    get_mean_action_duration(std::list<t_action> *history, uint8_t player_id) {
    uint32_t            n_action = 0;
    float               mean = 0.;
    for (std::list<t_action>::iterator it = history->begin(); it != history->end(); it++) {
        if ((*it).player->get_id() == player_id) {
            mean += (*it).duration.count();
            n_action++;
        }
    }
    mean /= n_action;
    return (uint32_t)mean;
}

Eigen::Array2i  Analytics::_handle_ratio(Eigen::Array2i pos) {
    return {(int32_t)(pos[0] * this->_res_ratio), (int32_t)(pos[1] * this->_res_ratio)};
}

void        Analytics::_update_analytics(bool init) {
    this->_data["g_time"] = {
        "time since start : ",
        format_time_since_start(std::chrono::steady_clock::now() - this->_game_engine->get_initial_timepoint()),
        this->_handle_ratio({10, 10})
    };
    this->_data["g_turn"] = {
        "current turn : ",
        std::to_string(this->_game_engine->get_history_size()),
        this->_handle_ratio({10, 30})
    };
    this->_data["p1"] = { "Player 1 - black", !init&&this->_c_player->get_id()==1?"   (current)":"", this->_handle_ratio({10, 100}) };
    this->_data["p1_captured_pairs"] = {
        "captured pairs : ",
        std::to_string(!init ? this->_player_1->get_pair_captured() : 0),
        this->_handle_ratio({15, 120})
    };
    this->_data["p1_mean_action_duration"] = {
        "mean action duration : ",
        format_duration(get_mean_action_duration(this->_game_engine->get_history(), 1)),
        this->_handle_ratio({15, 140})
    };
    this->_data["p2"] = { "Player 2 - white", !init&&this->_c_player->get_id()==2?"   (current)":"", this->_handle_ratio({10, 190}) };
    this->_data["p2_captured_pairs"] = {
        "captured pairs : ",
        std::to_string(!init ? this->_player_2->get_pair_captured() : 0),
        this->_handle_ratio({15, 210})
    };
    this->_data["p2_mean_action_duration"] = {
        "mean action duration : ",
        format_duration(get_mean_action_duration(this->_game_engine->get_history(), 2)),
        this->_handle_ratio({15, 230})
    };

    for (std::map<std::string,s_data>::iterator it=this->_data.begin(); it != this->_data.end(); it++) {
        it->second.text = it->second.pre + it->second.text;
    }
}

void        Analytics::render_text(void) {
    this->_update_analytics();
    this->_font_handler->render_text();
}
