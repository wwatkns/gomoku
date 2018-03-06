#ifndef GRAPHICALINTERFACE_HPP
# define GRAPHICALINTERFACE_HPP

# include <iostream>
# include <cmath>
# include <string>
# include <list>
# include <Eigen/Dense>
# include <SDL.h>
# include <SDL_image.h>
# include <SDL_ttf.h>
# include "GameEngine.hpp"
# include "FontHandler.hpp"
# include "Analytics.hpp"
# include "Button.hpp"
# include "ButtonSwitch.hpp"
# include "ButtonSelect.hpp"
// # include "MinMax.hpp"
// # include "SDL2_gfxPrimitives.h"

# define COLS 19
# define ROWS 19

class Player;
class Game;

class GraphicalInterface {

public:
    GraphicalInterface(GameEngine *game_engine);
    GraphicalInterface(GraphicalInterface const &src);
    ~GraphicalInterface(void);
    GraphicalInterface	&operator=(GraphicalInterface const &rhs);

    bool            check_newgame(void) { return _button_newgame->get_state(); };
    bool            check_restart(void) { return _button_restart->get_state(); };
    bool            check_pause(void) { return _button_pause->get_state(); };
    bool            check_undo(void) { return _button_undo->get_state(); };
    bool            check_close(void) { return this->_quit; };
    bool            check_mouse_on_board(void);
    void            update_events(void);
    void            update_display(void);
    void            update_end_game(Player const &p1, Player const &p2);

    SDL_Texture     *load_texture(std::string path);
    Eigen::Array2i  grid_to_screen(Eigen::Array2i pos);
    Eigen::Array2i  snap_to_grid(Eigen::Array2i pos);
    Eigen::Array2i  screen_to_grid(Eigen::Array2i pos);

    GameEngine      *get_game_engine(void) const { return _game_engine; };
    Analytics       *get_analytics(void) const { return _analytics; };
    Eigen::Array2i  get_mouse_pos(void) const { return _mouse_pos; };
    bool            get_mouse_action(void) const { return _mouse_action; };
    bool            get_end_game(void) const { return _end_game; };
    bool            get_nu(void) const { return _nu; };
    void            set_nu(bool const &val) { _nu = val; };

    std::string     render_choice_menu(void);

private:
    void            _init_sdl(void);
    void            _init_grid(void);
    void            _init_grid_points(void);
    void            _init_grid_indicators(void);
    void            _init_forbidden(void);

    void            _render_board(void);
    void            _render_stones(void);
    void            _render_stones_number(void);
    void            _render_forbidden(void);
    void            _render_select(void);
    void            _render_secondary_viewport(void);
    void            _render_buttons(void);
    void            _render_pause(void);
    void            _render_winning_screen(void);

    SDL_Rect        _handle_ratio(SDL_Rect rect);
    Eigen::Array2i  _handle_ratio(Eigen::Array2i pos);
    void            _load_images(void);
    void            _close_sdl(void);

    GameEngine      *_game_engine;
    Analytics       *_analytics;
    FontHandler     *_font_handler;

    SDL_Window      *_window;
    SDL_Renderer    *_renderer;
    SDL_Event       _event;

    SDL_Rect        _global_viewport;
    SDL_Rect        _main_viewport;
    SDL_Rect        _secondary_viewport;
    /* Board variables */
    int32_t         _win_w;
    int32_t         _win_h;
    int32_t         _res_w;
    int32_t         _res_h;
    float           _res_ratio;
    uint32_t        _grid_padding;
    int32_t         _stone_size;
    /* Helpers for coordinates convertions */
    Eigen::Array2i  _pad;
    Eigen::Array2f  _inc;
    /* Loaded textures */
    SDL_Texture     *_white_tex;
    SDL_Texture     *_white_stone_tex;
    SDL_Texture     *_black_stone_tex;
    SDL_Texture     *_select_stone_tex;
    SDL_Texture     *_forbidden_tex;
    SDL_Rect        _forbidden_rect;
    /* Computed textures */
    SDL_Texture     *_board_grid_tex;
    /* Event keys */
    const uint8_t   *_key_states;
    Eigen::Array2i  _mouse_pos;
    bool            _mouse_action;
    bool            _quit;
    /* buttons */
    Button          *_button_newgame;
    Button          *_button_restart;
    ButtonSwitch    *_button_pause;
    Button          *_button_undo;
    /* start menu buttons */
    ButtonSelect    *_menu_button_player_1;
    ButtonSelect    *_menu_button_player_2;

    /* gui colors */
    SDL_Color         _color_board_bg = { 215, 168,  84, 255 };
    SDL_Color       _color_board_grid = {   0,   0,   0, 255 };
    SDL_Color               _color_bg = {  15,  15,  15, 255 };
    SDL_Color              _color_win = {  45,  45,  45, 255 };
    SDL_Color           _color_header = {  35,  35,  35, 255 };
    SDL_Color           _color_button = {  53,  53,  53, 255 };
    SDL_Color             _color_font = { 160, 160, 160, 255 };
    SDL_Color           _color_font_2 = { 180, 180, 180, 255 };
    SDL_Color          _color_onhover = { 150, 150, 150, 255 };
    SDL_Color          _color_outline = {  70,  70,  70, 255 };

    SDL_Color            _color_white = { 255, 255, 255, 255 };
    SDL_Color            _color_black = {   0,   0,   0, 255 };
    SDL_Color             _color_gold = { 228, 161,  36, 255 };
    SDL_Color             _color_ruby = { 216,  17,  89, 255 };

    /* misc */
    bool            _nu;
    bool            _end_game;

    std::string     _stone_num_text;
    SDL_Color       _stone_num_color;
    FontText        *_stone_num_font_text;

    TTF_Font        *_default_font;
    SDL_Color       _winning_color;
    std::string     _winning_text;
    FontText        *_winning_font_text;
    TTF_Font        *_winning_font;
};

#endif
