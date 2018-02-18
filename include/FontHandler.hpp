#ifndef FONTHANDLER_HPP
# define FONTHANDLER_HPP

# include <iostream>
# include <string>
# include <vector>
# include <Eigen/Dense>
# include <SDL.h>
# include <SDL_ttf.h>
# include "FontText.hpp"

class FontHandler {

public:
    FontHandler(SDL_Renderer *renderer, float const res_ratio = 1.);
    FontHandler(FontHandler const &src);
    ~FontHandler(void);
    FontHandler	&operator=(FontHandler const &rhs);

    TTF_Font        *load_font(std::string path, uint32_t size = 28);
    void            create_text(std::string *text, Eigen::Array2i pos);
    void            create_text(std::string *text, Eigen::Array2i pos, TTF_Font *font, SDL_Color *color);
    void            create_text(std::string *text, Eigen::Array2i pos, std::string justify, TTF_Font *font, SDL_Color *color);
    void            render_text(void);

    /* getters */
    SDL_Renderer    *get_renderer(void) const { return _renderer; };
    /* setters */
    void            set_renderer(SDL_Renderer *renderer) { _renderer = renderer; };

    TTF_Font        *default_font;
    SDL_Color       *default_color;

private:
    SDL_Renderer            *_renderer;
    std::vector<FontText*>  _font_texts;

};

#endif
