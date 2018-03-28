#include "FontText.hpp"

FontText::FontText(std::string *text, Eigen::Array2i pos, TTF_Font *font, SDL_Color *color, SDL_Renderer *renderer) : _renderer(renderer), _renderer_texture(NULL), _text(text), _pos(pos), _font(font), _color(color) {
    this->_previous_text = "";
    this->_justify_h = 0;
    this->_justify_v = 0;
}

FontText::FontText(std::string *text, Eigen::Array2i pos, std::string justify_h, TTF_Font *font, SDL_Color *color, SDL_Renderer *renderer) : _renderer(renderer), _renderer_texture(NULL), _text(text), _pos(pos), _font(font), _color(color) {
    this->_previous_text = "";
    this->_justify_h = (justify_h.compare("right") == 0 ? 2 : (justify_h.compare("center") == 0 ? 1 : 0));
    this->_justify_v = 0;
}

FontText::FontText(std::string *text, Eigen::Array2i pos, std::string justify_h, std::string justify_v, TTF_Font *font, SDL_Color *color, SDL_Renderer *renderer) : _renderer(renderer), _renderer_texture(NULL), _text(text), _pos(pos), _font(font), _color(color) {
    this->_previous_text = "";
    this->_justify_h = (justify_h.compare("right")  == 0 ? 2 : (justify_h.compare("center") == 0 ? 1 : 0));
    this->_justify_v = (justify_v.compare("bottom") == 0 ? 2 : (justify_v.compare("center") == 0 ? 1 : 0));
}

FontText::FontText(FontText const &src) {
    *this = src;
}

FontText::~FontText(void) {
    SDL_DestroyTexture(this->_renderer_texture);
}

FontText	&FontText::operator=(FontText const &src) {
    this->_font = src.get_font();
    this->_text = src.get_text();
    this->_pos = src.get_pos();
    this->_renderer = src.get_renderer();
    this->_color = src.get_color();
    this->_renderer_texture = NULL;
    return (*this);
}

void        FontText::_justify_position(SDL_Rect *rect) {
    if (this->_justify_h != 0)
        rect->x -= (this->_justify_h == 1 ? rect->w / 2 : rect->w);
    if (this->_justify_v != 0)
        rect->y -= (this->_justify_v == 1 ? rect->h / 2 : rect->h);
}

void        FontText::_update_texture(void) {
    SDL_Surface *surf = TTF_RenderText_Blended(this->_font, this->_text->c_str(), *this->_color);
    if (this->_renderer_texture)
        SDL_DestroyTexture(this->_renderer_texture);
    this->_renderer_texture = SDL_CreateTextureFromSurface(this->_renderer, surf);
    SDL_FreeSurface(surf);
    surf = NULL;
}

void        FontText::render_text(void) {
    SDL_Rect    rect = (SDL_Rect){ this->_pos[0], this->_pos[1], 0, 0 };

    this->_update_texture();
    TTF_SizeText(this->_font, this->_text->c_str(), &rect.w, &rect.h);
    this->_justify_position(&rect);
    SDL_RenderCopyEx(this->_renderer, this->_renderer_texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
    SDL_DestroyTexture(this->_renderer_texture);
}

void        FontText::render_realtime_text(void) {
    /*  Only update the texture when the text has changed, best suited for text display such as analytics
        that change every so often (but definitely not every frame).
    */
    if (this->_previous_text.compare(*this->_text) != 0) {
        this->_update_texture();
        this->_renderer_rect = { this->_pos[0], this->_pos[1], 0, 0 };
        TTF_SizeText(this->_font, this->_text->c_str(), &this->_renderer_rect.w, &this->_renderer_rect.h);
        this->_justify_position(&this->_renderer_rect);
    }
    if (this->_renderer_texture != NULL) {
        SDL_RenderCopyEx(this->_renderer, this->_renderer_texture, NULL, &this->_renderer_rect, 0, NULL, SDL_FLIP_NONE);
    }
    this->_previous_text = *this->_text;
}
