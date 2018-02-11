#ifndef FONTHANDLER_HPP
# define FONTHANDLER_HPP

# include <iostream>
# include <string>
# include <Eigen/Dense>
# include <SDL.h>
# include <SDL_ttf.h>

class FontHandler {

public:
    FontHandler(SDL_Renderer *renderer, float const res_ratio = 1.);
    FontHandler(FontHandler const &src);
    ~FontHandler(void);
    FontHandler	&operator=(FontHandler const &rhs);

    TTF_Font        *load_font(std::string path, uint32_t size = 28);
    void            render_text(std::string *text, Eigen::Array2i *pos, TTF_Font *font);
    void            render_realtime_text(std::string *text, Eigen::Array2i *pos, TTF_Font *font);

    /* getters */
    SDL_Renderer    *get_renderer(void) const { return _renderer; };
    SDL_Color       get_render_color(void) const { return _render_color; };
    /* setters */
    void            set_renderer(SDL_Renderer *renderer) { _renderer = renderer; };
    void            set_render_color(SDL_Color color) { _render_color = color; };

    TTF_Font        *default_font;

private:
    SDL_Renderer    *_renderer;
    SDL_Texture     *_renderer_texture;
    SDL_Rect        _renderer_rect;
    SDL_Color       _render_color;

};

#endif
