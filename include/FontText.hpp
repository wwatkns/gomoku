#ifndef FONTTEXT_HPP
# define FONTTEXT_HPP

# include <iostream>
# include <string>
# include <Eigen/Dense>
# include <SDL.h>
# include <SDL_ttf.h>

class FontText {

public:
    FontText(std::string *text, Eigen::Array2i pos, TTF_Font *font, SDL_Color *color, SDL_Renderer *renderer);
    FontText(std::string *text, Eigen::Array2i pos, std::string justify_h, TTF_Font *font, SDL_Color *color, SDL_Renderer *renderer);
    FontText(std::string *text, Eigen::Array2i pos, std::string justify_h, std::string justify_v, TTF_Font *font, SDL_Color *color, SDL_Renderer *renderer);
    FontText(FontText const &src);
    ~FontText(void);
    FontText	&operator=(FontText const &rhs);

    void            render_text(void);
    void            render_realtime_text(void);

    /* getters */
    std::string     *get_text(void) const { return _text; };
    Eigen::Array2i  get_pos(void) const { return _pos; };
    TTF_Font        *get_font(void) const { return _font; };
    SDL_Renderer    *get_renderer(void) const { return _renderer; };
    SDL_Color       *get_color(void) const { return _color; };
    /* setters */
    void            set_font(TTF_Font *font) { _font = font; };
    void            set_text(std::string *text) { _text = text; };
    void            set_pos(Eigen::Array2i pos) { _pos = pos; };
    void            set_renderer(SDL_Renderer *renderer) { _renderer = renderer; };
    void            set_color(SDL_Color *color) { _color = color; };

private:
    SDL_Renderer    *_renderer;
    SDL_Texture     *_renderer_texture;
    std::string     *_text;
    Eigen::Array2i  _pos;
    uint8_t         _justify_h; // "left":0, "center":1,  "right":2
    uint8_t         _justify_v; //  "top":0, "center":1, "bottom":2
    TTF_Font        *_font;
    SDL_Color       *_color;
    std::string     _previous_text;
    SDL_Rect        _renderer_rect;

    void            _justify_position(SDL_Rect *rect);
    void            _update_texture(void);
};

#endif
