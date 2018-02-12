#include "FontText.hpp"

FontText::FontText(std::string *text, Eigen::Array2i *pos, TTF_Font *font, SDL_Color *color, SDL_Renderer *renderer) : _renderer(renderer), _text(text), _pos(pos), _font(font), _color(color), _renderer_texture(NULL) {
    std::cout << this->_text->c_str() << " " << (*this->_pos)[0] << " " << (*this->_pos)[1] << std::endl;
    this->_previous_text = "";
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

void        FontText::render_text(void) {
    SDL_Surface *surf = TTF_RenderText_Blended(this->_font, this->_text->c_str(), *this->_color);
    this->_renderer_texture = SDL_CreateTextureFromSurface(this->_renderer, surf);
    SDL_FreeSurface(surf);
    surf = NULL;

    SDL_Rect    rect = (SDL_Rect){ (*this->_pos)[0], (*this->_pos)[1], 0, 0 };
    TTF_SizeText(this->_font, this->_text->c_str(), &rect.w, &rect.h);
    SDL_RenderCopyEx(this->_renderer, this->_renderer_texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
    SDL_DestroyTexture(this->_renderer_texture);
}

void        FontText::render_realtime_text(void) {
    /*  Only update the texture when the text has changed, best suited for text display such as analytics
        that change every so often (but definitely not every frame).
        TODO : this'll work for one text, but for multiple texts it will not be an optimization...
    */
    if (this->_previous_text.compare(*this->_text) != 0) {
        SDL_Surface *surf = TTF_RenderText_Blended(this->_font, this->_text->c_str(), *this->_color);
        this->_renderer_texture = SDL_CreateTextureFromSurface(this->_renderer, surf);
        SDL_FreeSurface(surf);
        surf = NULL;

        this->_renderer_rect = { (*this->_pos)[0], (*this->_pos)[1], 0, 0 };
        TTF_SizeText(this->_font, this->_text->c_str(), &this->_renderer_rect.w, &this->_renderer_rect.h);
    }
    if (this->_renderer_texture != NULL) {
        SDL_RenderCopyEx(this->_renderer, this->_renderer_texture, NULL, &this->_renderer_rect, 0, NULL, SDL_FLIP_NONE);
    }
    this->_previous_text = *this->_text;
}
