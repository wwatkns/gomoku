#ifndef BUTTON_HPP
# define BUTTON_HPP

# include <iostream>
# include <string>
# include <Eigen/Dense>
# include <SDL.h>
# include <SDL_ttf.h>
# include "FontHandler.hpp"
# include "FontText.hpp"

class Button {

public:
    Button(SDL_Renderer *renderer, std::string text, Eigen::Array2i pos, Eigen::Array2i padding, TTF_Font *font, SDL_Color bg_color = {255, 255, 255, 255}, SDL_Color font_color = {0, 0, 0, 255}, SDL_Color hover_color = {231, 183, 136, 255}, SDL_Color outline_color = {0, 0, 0, 255});
    Button(Button const &src);
    ~Button(void);
    Button	&operator=(Button const &rhs);

    /* setters */
    void        set_state(bool state) { _state = state; };
    void        set_pos(Eigen::Array2i pos);
    void        set_padding(int32_t w, int32_t h) { _pad_w = w; _pad_h = h; };
    /* getters */
    bool            get_state(void) const { return _state; };
    SDL_Rect        get_rect(void) const { return _box_rect; };
    Eigen::Array2i  get_padding(void) const { return {_pad_w, _pad_h}; };

    bool        on_hover(Eigen::Array2i *pos);
    void        update_state(Eigen::Array2i *pos, bool mouse_press);
    void        render(SDL_Renderer *renderer, Eigen::Array2i *pos);

private:
    bool            _state;
    std::string     _text;
    SDL_Rect        _txt_rect;
    SDL_Rect        _box_rect;
    SDL_Color       _box_color;
    SDL_Color       _txt_color;
    SDL_Color       _hover_color;
    SDL_Color       _outline_color;
    int32_t         _pad_w;
    int32_t         _pad_h;
    TTF_Font        *_font;
    SDL_Texture     *_txt_texture;
    SDL_Texture     *_box_texture;
    SDL_Renderer    *_renderer;

    void            _draw_box(SDL_Color *box_color, SDL_Color *outline_color);

};

#endif
