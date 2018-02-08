#include "GraphicalInterface.hpp"

GraphicalInterface::GraphicalInterface(void) {
    this->_bg_color = (SDL_Color){215, 171, 84, 255};
    this->_init_sdl();
    this->_init_grid(8); // we will create this once and put it on a surface that we will update each time
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

void    GraphicalInterface::_init_sdl(void) {
    SDL_Init(SDL_INIT_VIDEO);

    this->_window = SDL_CreateWindow("Gomoku", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_H, WIN_W, SDL_WINDOW_SHOWN);
    this->_renderer = SDL_CreateRenderer(this->_window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawColor(this->_renderer, this->_bg_color.r, this->_bg_color.g, this->_bg_color.b, this->_bg_color.a);
}

void    GraphicalInterface::_init_grid(uint32_t padding) {
    uint32_t    pad_w = (uint32_t)(WIN_W * (float)(padding / 100.));
    uint32_t    pad_h = (uint32_t)(WIN_H * (float)(padding / 100.));
    float       inc_w = (float)(WIN_W - (pad_w * 2)) / COLS;
    float       inc_h = (float)(WIN_H - (pad_h * 2)) / ROWS;

    SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 255);
    for (int i = 0; i < ROWS + 1; i++) {
        SDL_RenderDrawLine(this->_renderer, pad_w, (int)(pad_h+i*inc_h), WIN_W - pad_w, (int)(pad_h+i*inc_h));
    }
    for (int i = 0; i < COLS + 1; i++) {
        SDL_RenderDrawLine(this->_renderer, (int)(pad_w+i*inc_w), pad_h, (int)(pad_h+i*inc_h), WIN_H - pad_h);
    }
}

void    GraphicalInterface::update_display(void) {
    SDL_SetRenderDrawColor(this->_renderer, this->_bg_color.r, this->_bg_color.g, this->_bg_color.b, this->_bg_color.a);
    SDL_RenderClear(this->_renderer);
    this->_init_grid(8);
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
    SDL_DestroyWindow(this->_window);
    this->_window = NULL;
    SDL_Quit();
}
