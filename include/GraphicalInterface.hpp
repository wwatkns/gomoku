#ifndef GRAPHICALINTERFACE_HPP
# define GRAPHICALINTERFACE_HPP

# include <iostream>
# include <cmath>
# include <string>
# include <list>
# include <Eigen/Dense>
# include <SDL.h>
# include <SDL_image.h>
// # include <SDL2_gfxPrimitives.h>
# include "GameEngine.hpp"
# include "FontHandler.hpp"
# include "Analytics.hpp"
# include "Button.hpp"

# define COLS 19
# define ROWS 19

class Player;

class GraphicalInterface {

public:
    GraphicalInterface(GameEngine *game_engine);
    GraphicalInterface(GraphicalInterface const &src);
    ~GraphicalInterface(void);
    GraphicalInterface	&operator=(GraphicalInterface const &rhs);

    bool            check_undo(void);
    bool            check_restart(void);
    bool            check_close(void);
    bool            check_mouse_action(void);
    bool            check_mouse_on_board(void);
    void            update_events(void);
    void            update_display(void);

    SDL_Texture     *load_texture(std::string path);
    Eigen::Array2i  grid_to_screen(Eigen::Array2i pos);
    Eigen::Array2i  snap_to_grid(Eigen::Array2i pos);
    Eigen::Array2i  screen_to_grid(Eigen::Array2i pos);

    GameEngine      *get_game_engine(void) const { return _game_engine; };
    Analytics       *get_analytics(void) const { return _analytics; };
    Eigen::Array2i  get_mouse_pos(void) const { return _mouse_pos; };
    bool            get_mouse_action(void) const { return _mouse_action; };

    void            render_choice_menu(void);

private:
    void    _init_sdl(void);
    void    _init_grid(void);
    void    _init_grid_points(void);
    void    _render_stones(void);
    void    _render_select(void);
    void    _render_secondary_viewport(void);
    void    _render_buttons(void);
    void    _load_images(void);
    void    _close_sdl(void);

    GameEngine      *_game_engine;
    Analytics       *_analytics;
    FontHandler     *_font_handler;

    SDL_Window      *_window;
    SDL_Renderer    *_renderer;
    SDL_Event       _event;
    SDL_Color       _bg_color;

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
    Button          *_button_undo;

};

#endif
