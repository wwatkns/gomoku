#ifndef ANALYTICS_HPP
# define ANALYTICS_HPP

# include <iostream>
# include <chrono>
# include <cmath>
# include <string>
# include <vector>
# include <map>
# include <Eigen/Dense>
# include <SDL.h>
# include "GameEngine.hpp"
# include "FontHandler.hpp"

// class Player;

typedef struct  s_data {
    std::string     pre;
    std::string     text;
    Eigen::Array2i  pos;
}               t_data;

class Analytics {

public:
    Analytics(GameEngine *game_engine, FontHandler *font_handler);
    Analytics(Analytics const &src);
    ~Analytics(void);
    Analytics	&operator=(Analytics const &rhs);

    void            render_text(void);

    GameEngine      *get_game_engine(void) const { return (_game_engine); };
    FontHandler     *get_font_handler(void) const { return (_font_handler); };

    void            set_players(Player *cp, Player *p1, Player *p2) { _c_player = cp; _player_1 = p1; _player_2 = p2; };
    void            set_c_player(Player *c_player) { _c_player = c_player; };
    void            set_player_1(Player *player_1) { _player_1 = player_1; };
    void            set_player_2(Player *player_2) { _player_2 = player_2; };

private:
    GameEngine              *_game_engine;
    FontHandler             *_font_handler;
    Player                  *_c_player;
    Player                  *_player_1;
    Player                  *_player_2;
    std::map<std::string,s_data>   _data;

    /* global analytics */
    t_data      _gl_turn;
    t_data      _gl_c_player;
    t_data      _gl_time;
    /* player 1 analytics */
    t_data      _p1_pairs_captured;
    t_data      _p1_mean_action_duration;
    /* player 2 analytics */
    t_data      _p2_pairs_captured;
    t_data      _p2_mean_action_duration;

    void            _update_analytics(void);

};

#endif
