#ifndef ANALYTICS_HPP
# define ANALYTICS_HPP

# include <iostream>
# include <chrono>
# include <cmath>
# include <string>
# include <vector>
# include <map>
# include <list>
# include <Eigen/Dense>
# include <SDL.h>
# include <SDL_ttf.h>
# include "GameEngine.hpp"
# include "FontHandler.hpp"
# include "Chronometer.hpp"

typedef struct  s_data {
    std::string     pre;
    std::string     text;
    TTF_Font        *font;
    SDL_Color       *color;
    Eigen::Array2i  pos;
    std::string     justify;
}               t_data;

class Analytics {

public:
    Analytics(GameEngine *game_engine, FontHandler *font_handler, float res_ratio);
    Analytics(Analytics const &src);
    ~Analytics(void);
    Analytics	&operator=(Analytics const &rhs);

    void            render_text(void);

    Chronometer     *get_chronometer(void) const { return (_chrono); };
    GameEngine      *get_game_engine(void) const { return (_game_engine); };
    FontHandler     *get_font_handler(void) const { return (_font_handler); };

    void            set_players(Player *cp, Player *p1, Player *p2) { _c_player = cp; _player_1 = p1; _player_2 = p2; };
    void            set_c_player(Player *c_player) { _c_player = c_player; };
    void            set_player_1(Player *player_1) { _player_1 = player_1; };
    void            set_player_2(Player *player_2) { _player_2 = player_2; };

private:
    GameEngine                      *_game_engine;
    FontHandler                     *_font_handler;
    Player                          *_c_player;
    Player                          *_player_1;
    Player                          *_player_2;
    Chronometer                     *_chrono;
    std::map<std::string,t_data>    _data;
    float                           _res_ratio;
    SDL_Color                       _font_color;

    TTF_Font                        *_font;
    TTF_Font                        *_font_title;
    TTF_Font                        *_font_small;
    SDL_Color                       _color_font = {160, 160, 160, 255};
    SDL_Color                       _color_font_title = {180, 180, 180, 255};
    SDL_Color                       _color_font_small = {130, 130, 130, 255};


    void            _update_analytics(bool init = false);
    Eigen::Array2i  _handle_ratio(Eigen::Array2i pos);
};

#endif
