#include "FontHandler.hpp"

FontHandler::FontHandler(SDL_Renderer *renderer, float const res_ratio) : _renderer(renderer) {
    this->default_font = this->load_font("./resources/fonts/Roboto-Regular.ttf", (int32_t)(14 * res_ratio));
    this->default_color = new (SDL_Color){ 0, 0, 0, 255 };
}

FontHandler::FontHandler(FontHandler const &src) {
    *this = src;
}

FontHandler::~FontHandler(void) {
    TTF_CloseFont(this->default_font);
}

FontHandler	&FontHandler::operator=(FontHandler const &src) {
    this->default_font = src.default_font;
    this->_renderer = src.get_renderer();
    return (*this);
}

TTF_Font    *FontHandler::load_font(std::string path, uint32_t size) {
    return (TTF_OpenFont(path.c_str(), size));
}

void        FontHandler::create_text(std::string *text, Eigen::Array2i pos) {
    create_text(text, pos, this->default_font, this->default_color);
}

void        FontHandler::create_text(std::string *text, Eigen::Array2i pos, TTF_Font *font, SDL_Color *color) {
    FontText    *new_text = new FontText(text, pos, font, color, this->_renderer);
    this->_font_texts.push_back(new_text);
}

void        FontHandler::create_text(std::string *text, Eigen::Array2i pos, std::string justify, TTF_Font *font, SDL_Color *color) {
    FontText    *new_text = new FontText(text, pos, justify, font, color, this->_renderer);
    this->_font_texts.push_back(new_text);
}

void        FontHandler::render_text(void) {
    for (uint32_t i = 0; i < this->_font_texts.size(); i++) {
        this->_font_texts[i]->render_realtime_text();
    }
}
