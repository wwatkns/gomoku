#ifndef GRAPHICALINTERFACE_HPP
# define GRAPHICALINTERFACE_HPP

# include <iostream>
# include <Eigen/Dense>
# include <SDL.h>
# include <SDL_image.h>
// # include <SDL2_gfxPrimitives.h>

# define WIN_H 1280
# define WIN_W 1280
# define COLS 19
# define ROWS 19

class GraphicalInterface {

public:
    GraphicalInterface(void);
    GraphicalInterface(GraphicalInterface const &src);
    ~GraphicalInterface(void);
    GraphicalInterface	&operator=(GraphicalInterface const &rhs);

    bool        check_close(void);
    void        update_display(void);
    void        add_stone(Eigen::Array2i pos, uint8_t pid);
    void        del_stone(Eigen::Array2i pos);

    SDL_Texture     *load_texture(std::string path);
    Eigen::Array2i  grid_to_screen(Eigen::Array2i pos);
    // Eigen::Array2i  screen_to_grid(Eigen::Array2i &pos);

private:
    void    _init_sdl(void);
    void    _init_grid(void);
    void    _init_grid_points(void);
    void    _load_images(void);

    // bool    _check_poll_event(void);
    void    _close_sdl(void);

    SDL_Window      *_window;
    SDL_Renderer    *_renderer;
    SDL_Event       _event;
    SDL_Color       _bg_color;
    uint32_t        _grid_padding;

    SDL_Texture     *_white_stone_tex;
    SDL_Texture     *_black_stone_tex;

    SDL_Texture     *_board_grid_tex;
    SDL_Texture     *_board_stones_tex;
};

#endif
