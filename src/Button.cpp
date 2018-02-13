#include "Button.hpp"

Button::Button(SDL_Renderer *renderer, std::string text, Eigen::Array2i pos, TTF_Font *font, SDL_Color color) : _renderer(renderer), _text(text), _font(font), _color(color) {
    this->_state = false;
    this->_text_color = { 0, 0, 0, 255 };
    this->_text_rect = { pos[0], pos[1], 0, 0 };
    TTF_SizeText(this->_font, this->_text.c_str(), &this->_text_rect.w, &this->_text_rect.h);

    std::cout << this->_text_rect.x << " " << this->_text_rect.y << " " << this->_text_rect.w << " " << this->_text_rect.h << std::endl;
    std::cout << (int)this->_color.r << " " << (int)this->_color.g << " " << (int)this->_color.b << " " << (int)this->_color.a << std::endl;
    std::cout << (int)this->_text_color.r << " " << (int)this->_text_color.g << " " << (int)this->_text_color.b << " " << (int)this->_text_color.a << std::endl;

    this->_rect = { pos[0], pos[1], (int32_t)(this->_text_rect.w), (int32_t)(this->_text_rect.h) }; // TODO : scale up

    this->_texture = SDL_CreateTexture(this->_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->_rect.w, this->_rect.h);
    SDL_SetTextureBlendMode(this->_texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(this->_renderer, this->_texture);
    SDL_RenderClear(this->_renderer);

    SDL_SetRenderDrawColor(this->_renderer, this->_color.r, this->_color.g, this->_color.b, this->_color.a);
    SDL_RenderFillRect(this->_renderer, &this->_rect);

    // SDL_Surface *srf = TTF_RenderText_Blended(this->_font, this->_text.c_str(), this->_text_color);
    SDL_Surface *srf = TTF_RenderText_Shaded(this->_font, this->_text.c_str(), this->_text_color, this->_color);
    this->_tmp_texture = SDL_CreateTextureFromSurface(this->_renderer, srf);
    SDL_RenderCopyEx(this->_renderer, this->_tmp_texture, NULL, &this->_text_rect, 0, NULL, SDL_FLIP_NONE);
    SDL_FreeSurface(srf);

    SDL_SetRenderTarget(this->_renderer, NULL);
}

Button::Button(Button const &src) {
    *this = src;
}

Button::~Button(void) {
}

Button	&Button::operator=(Button const &src) {
    return (*this);
}

void    Button::update_state(Eigen::Array2i *pos, bool mouse_press) {
    this->_state = false;
    SDL_SetTextureColorMod(this->_tmp_texture, 255, 255, 255);
    if ((*pos)[1] >= this->_rect.x && (*pos)[1] < this->_rect.x + this->_rect.w &&
        (*pos)[0] >= this->_rect.y && (*pos)[0] < this->_rect.y + this->_rect.h) {
        SDL_SetTextureColorMod(this->_tmp_texture, 128, 255, 255);
        if (mouse_press)
            this->_state = true;
    }
}

void    Button::render(SDL_Renderer *renderer) {
    SDL_RenderCopyEx(renderer, this->_tmp_texture, NULL, &this->_rect, 0, NULL, SDL_FLIP_NONE);
}
