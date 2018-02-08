#ifndef GRAPHICALINTERFACE_HPP
# define GRAPHICALINTERFACE_HPP

# include <iostream>
# include <SDL.h>

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

    bool    check_close(void);
    void    update_display(void);

private:
    void    _init_sdl(void);
    void    _init_grid(uint32_t padding);

    // bool    _check_poll_event(void);
    void    _close_sdl(void);

    SDL_Window      *_window;
    SDL_Renderer    *_renderer;
    SDL_Event       _event;
    SDL_Color       _bg_color;
};

#endif
