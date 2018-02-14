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
    Button(SDL_Renderer *renderer, std::string text, Eigen::Array2i pos, TTF_Font *font, SDL_Color bg_color);
    Button(Button const &src);
    ~Button(void);
    Button	&operator=(Button const &rhs);

    bool        get_state(void) const { return _state; };

    void        update_state(Eigen::Array2i *pos, bool mouse_press);
    void        render(SDL_Renderer *renderer);

private:
    bool            _state;
    std::string     _text;
    SDL_Rect        _txt_rect;
    SDL_Rect        _box_rect;
    SDL_Color       _box_color;
    SDL_Color       _txt_color;
    TTF_Font        *_font;
    SDL_Texture     *_txt_texture;
    SDL_Texture     *_box_texture;
    SDL_Renderer    *_renderer;

};

#endif
