#include "FontHandler.hpp"

FontHandler::FontHandler(SDL_Renderer *renderer, float const res_ratio) : _renderer(renderer), _renderer_texture(NULL) {
    TTF_Init();
    this->_render_color = (SDL_Color){ 0, 0, 0, 255 };
    this->default_font = this->load_font("./resources/fonts/Roboto-Regular.ttf", (int32_t)(14 * res_ratio));
}

FontHandler::FontHandler(FontHandler const &src) {
    *this = src;
}

FontHandler::~FontHandler(void) {
    TTF_CloseFont(this->default_font);
    SDL_DestroyTexture(this->_renderer_texture);
    TTF_Quit();
}

FontHandler	&FontHandler::operator=(FontHandler const &src) {
    this->default_font = src.default_font;
    this->_renderer = src.get_renderer();
    this->_render_color = src.get_render_color();
    this->_renderer_texture = NULL;
    return (*this);
}

TTF_Font    *FontHandler::load_font(std::string path, uint32_t size) {
    TTF_Font    *font;
    font = TTF_OpenFont(path.c_str(), size);
    return font;
}

void        FontHandler::render_text(std::string *text, Eigen::Array2i *pos, TTF_Font *font) {
    SDL_Surface *surf = TTF_RenderText_Blended(font, text->c_str(), this->_render_color);
    this->_renderer_texture = SDL_CreateTextureFromSurface(this->_renderer, surf);
    SDL_FreeSurface(surf);

    SDL_Rect    rect = (SDL_Rect){ (*pos)[0], (*pos)[1], 0, 0 };
    TTF_SizeText(font, text->c_str(), &rect.w, &rect.h);
    SDL_RenderCopyEx(this->_renderer, this->_renderer_texture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);
    SDL_DestroyTexture(this->_renderer_texture);
}

void        FontHandler::render_realtime_text(std::string *text, Eigen::Array2i *pos, TTF_Font *font) {
    /*  Only update the texture when the text has changed, best suited for text display such as analytics
        that change every so often (but definitely not every frame).
        TODO : this'll work for one text, but for multiple texts it will not be an optimization...
    */
    static std::string previous = "";

    if (previous.compare(*text) != 0) {
        SDL_Surface *surf = TTF_RenderText_Blended(font, text->c_str(), this->_render_color);
        this->_renderer_texture = SDL_CreateTextureFromSurface(this->_renderer, surf);
        SDL_FreeSurface(surf);

        this->_renderer_rect = { (*pos)[0], (*pos)[1], 0, 0 };
        TTF_SizeText(font, text->c_str(), &this->_renderer_rect.w, &this->_renderer_rect.h);
    }
    if (this->_renderer_texture != NULL) {
        SDL_RenderCopyEx(this->_renderer, this->_renderer_texture, NULL, &this->_renderer_rect, 0, NULL, SDL_FLIP_NONE);
    }
    previous = *text;
}
