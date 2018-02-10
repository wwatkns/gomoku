#ifndef GRAPHICALINTERFACE_HPP
# define GRAPHICALINTERFACE_HPP

# include <iostream>
# include <cmath>
# include <Eigen/Dense>
# include <SDL.h>
# include <SDL_image.h>
// # include <SDL2_gfxPrimitives.h>
# include "GameEngine.hpp"

# define COLS 19
# define ROWS 19

class GraphicalInterface {

public:
    GraphicalInterface(GameEngine *game_engine);
    GraphicalInterface(GraphicalInterface const &src);
    ~GraphicalInterface(void);
    GraphicalInterface	&operator=(GraphicalInterface const &rhs);

    bool            check_close(void);
    bool            check_mouse_action(void);
    void            update_events(void);
    void            update_display(void);

    SDL_Texture     *load_texture(std::string path);
    Eigen::Array2i  grid_to_screen(Eigen::Array2i pos);
    Eigen::Array2i  snap_to_grid(Eigen::Array2i pos);
    Eigen::Array2i  screen_to_grid(Eigen::Array2i pos);

    GameEngine      *get_game_engine(void) const;
    Eigen::Array2i  get_mouse_pos(void) const;

private:
    void    _init_sdl(void);
    void    _init_grid(void);
    void    _init_grid_points(void);
    void    _render_stones(void);
    void    _render_select(void);
    void    _load_images(void);
    void    _close_sdl(void);

    GameEngine      *_game_engine;

    SDL_Window      *_window;
    SDL_Renderer    *_renderer;
    SDL_Event       _event;
    SDL_Color       _bg_color;
    /* Board variables */
    int32_t         _win_w;
    int32_t         _win_h;
    int32_t         _res_w;
    int32_t         _res_h;
    float           _win_res_ratio;
    uint32_t        _grid_padding;
    int32_t         _stone_size;
    /* Helpers for coordinates convertions */
    Eigen::Array2i  _pad;
    Eigen::Array2f  _inc;
    /* Loaded textures */
    SDL_Texture     *_white_stone_tex;
    SDL_Texture     *_black_stone_tex;
    SDL_Texture     *_select_stone_tex;
    /* Computed textures */
    SDL_Texture     *_board_grid_tex;
    /* Event keys */
    const uint8_t*  _key_states;
    Eigen::Array2i  _mouse_pos;
    bool            _mouse_action;
    bool            _quit;

};

#endif
