#include "GraphicalInterface.hpp"

GraphicalInterface::GraphicalInterface(void) {
    this->_grid_padding = 8;
    this->_bg_color = (SDL_Color){215, 171, 84, 255};
    this->_init_sdl();
    this->_load_images();
    this->_init_grid(); // we will create this once and put it on a surface that we will update each time
    // we will have a surface for the stones too
}

GraphicalInterface::GraphicalInterface(GraphicalInterface const &src) {
    *this = src;
}

GraphicalInterface::~GraphicalInterface(void) {
    this->_close_sdl();
}

GraphicalInterface	&GraphicalInterface::operator=(GraphicalInterface const &src) {
    return (*this);
}

SDL_Texture *GraphicalInterface::load_texture(std::string path) {
    IMG_Init(IMG_INIT_PNG);

    SDL_Texture *texture = NULL;
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());

    texture = SDL_CreateTextureFromSurface(this->_renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
    return texture;
}

void    GraphicalInterface::_init_sdl(void) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    this->_window = SDL_CreateWindow("gomoku", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_H, WIN_W, SDL_WINDOW_SHOWN);
    // this->_screen_surface = SDL_GetWindowSurface(this->_window);
    this->_renderer = SDL_CreateRenderer(this->_window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(this->_renderer, this->_bg_color.r, this->_bg_color.g, this->_bg_color.b, this->_bg_color.a);
}

void    GraphicalInterface::_load_images(void) {
    this->_white_stone_surface = this->load_texture(std::string("./resources/circle_white.png"));
    this->_black_stone_surface = this->load_texture(std::string("./resources/circle_black.png"));
}

void    GraphicalInterface::_init_grid(void) {
    uint32_t    pad_w = (uint32_t)(WIN_W * (float)(this->_grid_padding / 100.));
    uint32_t    pad_h = (uint32_t)(WIN_H * (float)(this->_grid_padding / 100.));
    float       inc_w = (float)(WIN_W - (pad_w * 2)) / (COLS-1);
    float       inc_h = (float)(WIN_H - (pad_h * 2)) / (ROWS-1);

    SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 255);
    for (int i = 0; i < ROWS; i++) {
        SDL_RenderDrawLine(this->_renderer, pad_w, (int)(pad_h+i*inc_h), WIN_W - pad_w, (int)(pad_h+i*inc_h));
    }
    for (int i = 0; i < COLS; i++) {
        SDL_RenderDrawLine(this->_renderer, (int)(pad_w+i*inc_w), pad_h, (int)(pad_h+i*inc_h), WIN_H - pad_h);
    }
}

void    GraphicalInterface::update_display(void) {
    SDL_SetRenderDrawColor(this->_renderer, this->_bg_color.r, this->_bg_color.g, this->_bg_color.b, this->_bg_color.a);
    SDL_RenderClear(this->_renderer);
    this->_init_grid(); // tmp

    SDL_Rect    pos = {WIN_W/2-27, WIN_H/2-27, 54, 54};

    SDL_RenderCopy(this->_renderer, this->_white_stone_surface, NULL, &pos);
    SDL_RenderPresent(this->_renderer);
}

bool    GraphicalInterface::check_close(void) {
    bool    quit = false;

    while (SDL_PollEvent(&this->_event) != 0) {
        if(this->_event.type == SDL_QUIT)
            quit = true;
    }
    return quit;
}

void    GraphicalInterface::_close_sdl(void) {
    SDL_DestroyTexture(this->_white_stone_surface);
    SDL_DestroyTexture(this->_black_stone_surface);
    this->_white_stone_surface = NULL;
    this->_black_stone_surface = NULL;

    SDL_DestroyWindow(this->_window);
    SDL_DestroyRenderer(this->_renderer);
    this->_window = NULL;
    this->_renderer = NULL;

    IMG_Quit();
    SDL_Quit();
}
