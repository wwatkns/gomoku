#include "Analytics.hpp"
#include "Player.hpp"

Analytics::Analytics(GameEngine *game_engine, FontHandler *font_handler) : _game_engine(game_engine), _font_handler(font_handler), _player_1(nullptr), _player_2(nullptr) {
    this->_update_analytics();
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

static std::string  format_duration(std::chrono::duration<double> elapsed) {
    std::string formated_time;
    uint32_t    seconds;

    seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
    formated_time = fzeros(std::to_string(seconds/60), 2)+":"+fzeros(std::to_string(seconds%60), 2)+" min";
    return formated_time;
}

void        Analytics::_update_analytics(void) {
    this->_data["g_time"] = {
        "time since start : ",
        format_duration(std::chrono::steady_clock::now() - this->_game_engine->get_initial_timepoint()),
        {5, 10}
    };
    this->_data["g_turn"] = {
        "current turn : ",
        std::to_string(this->_game_engine->get_history_size()),
        {5, 30}
    };
    this->_data["g_cplayer"] = {
        "current player : ",
        // std::to_string(this->_game_engine->get_history()->back().player_id),
        std::to_string(this->_c_player ? this->_c_player->get_id() : 0),
        {5, 50}
    };
    this->_data["p1_captured_pairs"] = {
        "captured pairs : ",
        std::to_string(this->_player_1 ? this->_player_1->get_pair_captured() : 0),
        {5, 70}
    };
    // this->_data["p1_mean_action_duration"] = { "mean action duration : ", std::to_string(this->_game_engine->get_history_size()), {5, 90} };
    this->_data["p2_captured_pairs"] = {
        "captured pairs : ",
        std::to_string(this->_player_2 ? this->_player_2->get_pair_captured() : 0),
        {5, 110}
    };
    // this->_data["p2_mean_action_duration"] = { "mean action duration : ", std::to_string(this->_game_engine->get_history_size()), {5, 130} };

    for (std::map<std::string,s_data>::iterator it=this->_data.begin(); it != this->_data.end(); it++) {
        it->second.text = it->second.pre + it->second.text;
    }
}

void        Analytics::render_text(void) {
    this->_update_analytics();
    this->_font_handler->render_text();
}
