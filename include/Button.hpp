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
    Button(SDL_Renderer *renderer, std::string text, Eigen::Array2i pos, TTF_Font *font, SDL_Color color);
    Button(Button const &src);
    ~Button(void);
    Button	&operator=(Button const &rhs);

    bool        get_state(void) const { return _state; };

    void        update_state(Eigen::Array2i *pos, bool mouse_press);
    void        render(SDL_Renderer *renderer);

private:
    bool            _state;
    std::string     _text;
    SDL_Rect        _text_rect;
    SDL_Rect        _rect;
    SDL_Color       _color;
    SDL_Color       _text_color;
    TTF_Font        *_font;
    SDL_Texture     *_text_texture;
    SDL_Texture     *_box_texture;
    SDL_Renderer    *_renderer;

};

#endif
