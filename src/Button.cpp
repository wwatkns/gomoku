#include "Button.hpp"

Button::Button(SDL_Renderer *renderer, std::string text, Eigen::Array2i pos, TTF_Font *font, SDL_Color bg_color) : _renderer(renderer), _text(text), _font(font), _box_color(bg_color) {
    uint8_t w = 12;
    uint8_t h = 2;
    this->_state = false;
    this->_txt_color = { 0, 0, 0, 255 };
    this->_txt_rect = { pos[0] + w / 2, pos[1] + h / 2, 0, 0 };
    TTF_SizeText(this->_font, this->_text.c_str(), &this->_txt_rect.w, &this->_txt_rect.h);

    this->_box_rect = { pos[0], pos[1], (int32_t)(this->_txt_rect.w + w), (int32_t)(this->_txt_rect.h + h) };

    this->_box_texture = SDL_CreateTexture(this->_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->_box_rect.w, this->_box_rect.h);
    SDL_SetRenderTarget(this->_renderer, this->_box_texture);
    SDL_SetRenderDrawColor(this->_renderer, this->_box_color.r, this->_box_color.g, this->_box_color.b, this->_box_color.a);
    SDL_RenderClear(this->_renderer);
    SDL_SetRenderTarget(this->_renderer, NULL);

    SDL_Surface *srf = TTF_RenderText_Blended(this->_font, this->_text.c_str(), this->_txt_color);
    this->_txt_texture = SDL_CreateTextureFromSurface(this->_renderer, srf);
    SDL_RenderCopyEx(this->_renderer, this->_txt_texture, NULL, &this->_txt_rect, 0, NULL, SDL_FLIP_NONE);
    SDL_FreeSurface(srf);
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
    SDL_SetTextureColorMod(this->_txt_texture, 255, 255, 255);
    SDL_SetTextureColorMod(this->_box_texture, 255, 255, 255);
    if ((*pos)[1] >= this->_box_rect.x && (*pos)[1] < this->_box_rect.x + this->_box_rect.w &&
        (*pos)[0] >= this->_box_rect.y && (*pos)[0] < this->_box_rect.y + this->_box_rect.h) {
        SDL_SetTextureColorMod(this->_txt_texture, 231, 183, 136);
        SDL_SetTextureColorMod(this->_box_texture, 231, 183, 136);
        if (mouse_press)
            this->_state = true;
    }
}

void    Button::render(SDL_Renderer *renderer) {
    SDL_RenderCopyEx(renderer, this->_box_texture, NULL, &this->_box_rect, 0, NULL, SDL_FLIP_NONE);
    SDL_RenderCopyEx(renderer, this->_txt_texture, NULL, &this->_txt_rect, 0, NULL, SDL_FLIP_NONE);
}
