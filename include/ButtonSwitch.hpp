#ifndef BUTTONSWITCH_HPP
# define BUTTONSWITCH_HPP

# include <iostream>
# include <string>
# include <Eigen/Dense>
# include <SDL.h>
# include <SDL_ttf.h>
# include "FontHandler.hpp"
# include "FontText.hpp"

class ButtonSwitch {

public:
    ButtonSwitch(SDL_Renderer *renderer, std::string text_idle, std::string text_active, Eigen::Array2i pos, Eigen::Array2i padding, TTF_Font *font, SDL_Color bg_color = {255, 255, 255, 255}, SDL_Color font_color = {0, 0, 0, 255}, SDL_Color active_color = {231, 183, 136, 255}, SDL_Color outline_color = {0, 0, 0, 255});
    ButtonSwitch(ButtonSwitch const &src);
    ~ButtonSwitch(void);
    ButtonSwitch	&operator=(ButtonSwitch const &rhs);

    /* setters */
    void        set_state(bool state) { _state = state; };
    void        set_pos_idle(Eigen::Array2i pos);
    void        set_pos_active(Eigen::Array2i pos);
    void        set_padding(int32_t w, int32_t h) { _pad_w = w; _pad_h = h; };
    /* getters */
    bool            get_state(void) const { return _state; };
    Eigen::Array2i  get_padding(void) const { return {_pad_w, _pad_h}; };

    bool        on_hover(Eigen::Array2i *pos);
    void        update_state(Eigen::Array2i *pos, bool mouse_press);
    void        render(SDL_Renderer *renderer, Eigen::Array2i *pos);

private:
    bool            _state;
    std::string     _text_idle;
    std::string     _text_active;
    SDL_Rect        _txt_idle_rect;
    SDL_Rect        _txt_active_rect;
    SDL_Rect        _box_idle_rect;
    SDL_Rect        _box_active_rect;
    SDL_Color       _box_color;
    SDL_Color       _txt_color;
    SDL_Color       _active_color;
    SDL_Color       _outline_color;
    int32_t         _pad_w;
    int32_t         _pad_h;
    TTF_Font        *_font;
    SDL_Texture     *_txt_idle_texture;
    SDL_Texture     *_txt_active_texture;
    SDL_Texture     *_box_idle_texture;
    SDL_Texture     *_box_active_texture;
    SDL_Renderer    *_renderer;

    void            _draw_box_idle(SDL_Color *box_color, SDL_Color *outline_color);
    void            _draw_box_active(SDL_Color *box_color, SDL_Color *outline_color);

};

#endif
