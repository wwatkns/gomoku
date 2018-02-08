#ifndef GRAPHICALINTERFACE_HPP
# define GRAPHICALINTERFACE_HPP

# include <iostream>
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

    SDL_Texture *load_texture(std::string path);
    // TODO: function to convert `grid_pos` to `screen_pos`

private:
    void    _init_sdl(void);
    void    _load_images(void);
    void    _init_grid(void);

    // bool    _check_poll_event(void);
    void    _close_sdl(void);

    SDL_Window      *_window;
    SDL_Renderer    *_renderer;
    SDL_Event       _event;
    SDL_Color       _bg_color;
    uint32_t        _grid_padding;

    SDL_Texture     *_white_stone_surface;
    SDL_Texture     *_black_stone_surface;
};

#endif
