#include "Button.hpp"

Button::Button(SDL_Renderer *renderer, std::string text, Eigen::Array2i pos, Eigen::Array2i padding, TTF_Font *font, SDL_Color bg_color, SDL_Color font_color, SDL_Color hover_color, SDL_Color outline_color) : _renderer(renderer), _text(text), _font(font), _box_color(bg_color), _txt_color(font_color), _hover_color(hover_color), _outline_color(outline_color) {
    this->_pad_w = padding[0];
    this->_pad_h = padding[1];
    this->_state = false;
    this->set_pos(pos);

    this->_box_texture = SDL_CreateTexture(this->_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->_box_rect.w, this->_box_rect.h);
    this->_draw_box(&this->_box_color, &this->_outline_color);
    this->set_pos(pos);

    SDL_Surface *srf = TTF_RenderText_Blended(this->_font, this->_text.c_str(), this->_txt_color);
    this->_txt_texture = SDL_CreateTextureFromSurface(this->_renderer, srf);
    SDL_RenderCopyEx(this->_renderer, this->_txt_texture, NULL, &this->_txt_rect, 0, NULL, SDL_FLIP_NONE);
    SDL_FreeSurface(srf);
}

Button::Button(Button const &src) {
    *this = src;
}

Button::~Button(void) {
    SDL_DestroyTexture(this->_txt_texture);
    SDL_DestroyTexture(this->_box_texture);
}

Button	&Button::operator=(Button const &src) {
    return (*this);
}

void    Button::_draw_box(SDL_Color *box_color, SDL_Color *outline_color) {
    SDL_SetRenderTarget(this->_renderer, this->_box_texture);

    /* draw box bg */
    SDL_SetRenderDrawColor(this->_renderer, box_color->r, box_color->g, box_color->b, box_color->a);
    SDL_RenderClear(this->_renderer);
    /* draw outline */
    SDL_SetRenderDrawColor(this->_renderer, outline_color->r, outline_color->g, outline_color->b, outline_color->a);
    this->_box_rect.x = 0;
    this->_box_rect.y = 0;
    SDL_RenderDrawRect(this->_renderer, &this->_box_rect);
    // std::cout << this->_box_rect.x << " " << this->_box_rect.y << " " << this->_box_rect.w << " " << this->_box_rect.h << std::endl;
    SDL_SetRenderTarget(this->_renderer, NULL);
}

void    Button::set_pos(Eigen::Array2i pos) {
    this->_txt_rect = { pos[0] + this->_pad_w / 2, pos[1] + this->_pad_h / 2, 0, 0 };
    TTF_SizeText(this->_font, this->_text.c_str(), &this->_txt_rect.w, &this->_txt_rect.h);
    this->_box_rect = { pos[0], pos[1], (int32_t)(this->_txt_rect.w + this->_pad_w), (int32_t)(this->_txt_rect.h + this->_pad_h) };
}

bool    Button::on_hover(Eigen::Array2i *pos) {
    return ((*pos)[1] >= this->_box_rect.x && (*pos)[1] < this->_box_rect.x + this->_box_rect.w &&
            (*pos)[0] >= this->_box_rect.y && (*pos)[0] < this->_box_rect.y + this->_box_rect.h);
}

void    Button::update_state(Eigen::Array2i *pos, bool mouse_press) {
    this->_state = false;
    if (this->on_hover(pos) && mouse_press == true) {
        this->_state = true;
    }
}

void    Button::render(SDL_Renderer *renderer, Eigen::Array2i *pos) {
    if (this->on_hover(pos) || this->_state == true) {
        SDL_SetTextureColorMod(this->_box_texture, this->_hover_color.r, this->_hover_color.g, this->_hover_color.b);
    } else {
        SDL_SetTextureColorMod(this->_box_texture, 255, 255, 255);
    }
    SDL_RenderCopy(renderer, this->_box_texture, NULL, &this->_box_rect);
    SDL_RenderCopy(renderer, this->_txt_texture, NULL, &this->_txt_rect);
}
