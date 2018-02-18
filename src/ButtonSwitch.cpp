#include "ButtonSwitch.hpp"

ButtonSwitch::ButtonSwitch(SDL_Renderer *renderer, std::string text_idle, std::string text_active, Eigen::Array2i pos, Eigen::Array2i padding, TTF_Font *font, SDL_Color bg_color, SDL_Color font_color, SDL_Color active_color, SDL_Color outline_color) : _renderer(renderer), _text_idle(text_idle), _text_active(text_active), _font(font), _box_color(bg_color), _txt_color(font_color), _active_color(active_color), _outline_color(outline_color) {
    this->_pad_w = padding[0];
    this->_pad_h = padding[1];
    this->_state = false;
    this->set_pos_idle(pos);
    this->set_pos_active(pos);

    this->_box_idle_texture = SDL_CreateTexture(this->_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->_box_idle_rect.w, this->_box_idle_rect.h);
    this->_box_active_texture = SDL_CreateTexture(this->_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->_box_active_rect.w, this->_box_active_rect.h);
    this->_draw_box_idle(&this->_box_color, &this->_outline_color);
    this->_draw_box_active(&this->_box_color, &this->_outline_color);

    SDL_Surface *srf = TTF_RenderText_Blended(this->_font, this->_text_idle.c_str(), this->_txt_color);
    this->_txt_idle_texture = SDL_CreateTextureFromSurface(this->_renderer, srf);
    SDL_FreeSurface(srf);
    srf = TTF_RenderText_Blended(this->_font, this->_text_active.c_str(), this->_txt_color);
    this->_txt_active_texture = SDL_CreateTextureFromSurface(this->_renderer, srf);
    SDL_RenderCopyEx(this->_renderer, this->_txt_idle_texture, NULL, &this->_txt_idle_rect, 0, NULL, SDL_FLIP_NONE);
    SDL_FreeSurface(srf);
}

ButtonSwitch::ButtonSwitch(ButtonSwitch const &src) {
    *this = src;
}

ButtonSwitch::~ButtonSwitch(void) {
    SDL_DestroyTexture(this->_txt_idle_texture);
    SDL_DestroyTexture(this->_box_idle_texture);
    SDL_DestroyTexture(this->_txt_active_texture);
    SDL_DestroyTexture(this->_box_active_texture);
}

ButtonSwitch	&ButtonSwitch::operator=(ButtonSwitch const &src) {
    return (*this);
}

void    ButtonSwitch::_draw_box_idle(SDL_Color *box_color, SDL_Color *outline_color) {
    SDL_SetRenderTarget(this->_renderer, this->_box_idle_texture);
    /* draw box bg */
    SDL_SetRenderDrawColor(this->_renderer, box_color->r, box_color->g, box_color->b, box_color->a);
    SDL_RenderClear(this->_renderer);
    /* draw outline */
    SDL_Rect    rect = { 0, 0, this->_box_idle_rect.w, this->_box_idle_rect.h };
    SDL_SetRenderDrawColor(this->_renderer, outline_color->r, outline_color->g, outline_color->b, outline_color->a);
    SDL_RenderDrawRect(this->_renderer, &rect);
    SDL_SetRenderTarget(this->_renderer, NULL);
}

void    ButtonSwitch::_draw_box_active(SDL_Color *box_color, SDL_Color *outline_color) {
    SDL_SetRenderTarget(this->_renderer, this->_box_active_texture);
    /* draw box bg */
    SDL_SetRenderDrawColor(this->_renderer, box_color->r, box_color->g, box_color->b, box_color->a);
    SDL_RenderClear(this->_renderer);
    /* draw outline */
    SDL_Rect    rect = { 0, 0, this->_box_active_rect.w, this->_box_active_rect.h };
    SDL_SetRenderDrawColor(this->_renderer, outline_color->r, outline_color->g, outline_color->b, outline_color->a);
    SDL_RenderDrawRect(this->_renderer, &rect);
    SDL_SetRenderTarget(this->_renderer, NULL);
}

void    ButtonSwitch::set_pos_idle(Eigen::Array2i pos) {
    this->_txt_idle_rect = { pos[0] + this->_pad_w / 2, pos[1] + this->_pad_h / 2, 0, 0 };
    TTF_SizeText(this->_font, this->_text_idle.c_str(), &this->_txt_idle_rect.w, &this->_txt_idle_rect.h);
    this->_box_idle_rect = { pos[0], pos[1], (int32_t)(this->_txt_idle_rect.w + this->_pad_w), (int32_t)(this->_txt_idle_rect.h + this->_pad_h) };
}

void    ButtonSwitch::set_pos_active(Eigen::Array2i pos) {
    this->_txt_active_rect = { pos[0] + this->_pad_w / 2, pos[1] + this->_pad_h / 2, 0, 0 };
    TTF_SizeText(this->_font, this->_text_active.c_str(), &this->_txt_active_rect.w, &this->_txt_active_rect.h);
    this->_box_active_rect = { pos[0], pos[1], (int32_t)(this->_txt_active_rect.w + this->_pad_w), (int32_t)(this->_txt_active_rect.h + this->_pad_h) };
}

bool    ButtonSwitch::on_hover(Eigen::Array2i *pos) {
    SDL_Rect    *current = (this->_state ? &this->_box_active_rect : &this->_box_idle_rect);
    return ((*pos)[1] >= current->x && (*pos)[1] < current->x + current->w &&
            (*pos)[0] >= current->y && (*pos)[0] < current->y + current->h);
}

void    ButtonSwitch::update_state(Eigen::Array2i *pos, bool mouse_press) {
    if (this->on_hover(pos) && mouse_press == true)
        this->_state = (this->_state ? false : true);
}

void    ButtonSwitch::render(SDL_Renderer *renderer, Eigen::Array2i *pos) {
    SDL_Texture *current_box_texture = (this->_state ? this->_box_active_texture : this->_box_idle_texture);
    SDL_Texture *current_txt_texture = (this->_state ? this->_txt_active_texture : this->_txt_idle_texture);
    SDL_Rect    *current_box_rect = (this->_state ? &this->_box_active_rect : &this->_box_idle_rect);
    SDL_Rect    *current_txt_rect = (this->_state ? &this->_txt_active_rect : &this->_txt_idle_rect);

    if (this->on_hover(pos) || this->_state == true) {
        SDL_SetTextureColorMod(current_box_texture, this->_active_color.r, this->_active_color.g, this->_active_color.b);
    } else {
        SDL_SetTextureColorMod(current_box_texture, 255, 255, 255);
    }
    SDL_RenderCopy(renderer, current_box_texture, NULL, current_box_rect);
    SDL_RenderCopy(renderer, current_txt_texture, NULL, current_txt_rect);
}
