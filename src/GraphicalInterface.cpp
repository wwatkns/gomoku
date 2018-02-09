#include "GraphicalInterface.hpp"

GraphicalInterface::GraphicalInterface(void) {
    this->_grid_padding = 8;
    this->_bg_color = (SDL_Color){215, 171, 84, 255};
    this->_init_sdl();
    this->_load_images();
    this->_init_grid();

    this->add_stone((Eigen::Array2i){10, 3}, 1);
    this->add_stone((Eigen::Array2i){11, 3}, 1);
    this->add_stone((Eigen::Array2i){9, 4}, 2);
    this->add_stone((Eigen::Array2i){10, 4}, 2);
    this->add_stone((Eigen::Array2i){9, 3}, 1);
    this->add_stone((Eigen::Array2i){14, 1}, 2);

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

Eigen::Array2i  GraphicalInterface::grid_to_screen(Eigen::Array2i pos) {
    uint32_t    pad_w = (uint32_t)(WIN_W * (float)(this->_grid_padding / 100.));
    uint32_t    pad_h = (uint32_t)(WIN_H * (float)(this->_grid_padding / 100.));
    float       inc_w = (float)(WIN_W - (pad_w * 2)) / (COLS-1);
    float       inc_h = (float)(WIN_H - (pad_h * 2)) / (ROWS-1);

    Eigen::Array2i  screen_pos;
    screen_pos << pad_h + pos[0] * inc_h, pad_w + pos[1] * inc_w;
    return screen_pos;
}

void    GraphicalInterface::_init_sdl(void) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    this->_window = SDL_CreateWindow("gomoku", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_H, WIN_W, SDL_WINDOW_SHOWN);
    this->_renderer = SDL_CreateRenderer(this->_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    // SDL_SetRenderDrawColor(this->_renderer, this->_bg_color.r, this->_bg_color.g, this->_bg_color.b, this->_bg_color.a);
    this->_board_grid_tex = SDL_CreateTexture(this->_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIN_W, WIN_H);
    this->_board_stones_tex = SDL_CreateTexture(this->_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIN_W, WIN_H);

    SDL_SetRenderDrawBlendMode(this->_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(this->_board_grid_tex, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(this->_board_stones_tex, SDL_BLENDMODE_BLEND);
}

void    GraphicalInterface::_load_images(void) {
    this->_white_stone_tex = this->load_texture(std::string("./resources/circle_white.png"));
    this->_black_stone_tex = this->load_texture(std::string("./resources/circle_black.png"));
}

void    GraphicalInterface::_init_grid(void) {
    uint32_t    pad_w = (uint32_t)(WIN_W * (float)(this->_grid_padding / 100.));
    uint32_t    pad_h = (uint32_t)(WIN_H * (float)(this->_grid_padding / 100.));
    float       inc_w = (float)(WIN_W - (pad_w * 2)) / (COLS-1);
    float       inc_h = (float)(WIN_H - (pad_h * 2)) / (ROWS-1);

    /* will render to the texture _board_grid_tex */
    SDL_SetRenderTarget(this->_renderer, this->_board_grid_tex);
    SDL_RenderClear(this->_renderer);

    SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 255);
    for (int i = 0; i < ROWS; i++) {
        SDL_RenderDrawLine(this->_renderer, pad_w, (int)(pad_h+i*inc_h), WIN_W - pad_w, (int)(pad_h+i*inc_h));
    }
    for (int i = 0; i < COLS; i++) {
        SDL_RenderDrawLine(this->_renderer, (int)(pad_w+i*inc_w), pad_h, (int)(pad_h+i*inc_h), WIN_H - pad_h);
    }
    this->_init_grid_points();
    /* reset the renderer target to the screen */
    SDL_SetRenderTarget(this->_renderer, NULL);
}

void    GraphicalInterface::_init_grid_points(void) {
    Eigen::Array2i  pos;
    SDL_Rect        rect;

    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < 3; i++) {
            pos = this->grid_to_screen((Eigen::Array2i){3+j*6, 3+i*6});
            rect = {pos[1]-2, pos[0]-2, 5, 5};
            SDL_RenderCopy(this->_renderer, this->_black_stone_tex, NULL, &rect);
        }
    }
}


void    GraphicalInterface::update_display(void) {
    SDL_SetRenderDrawColor(this->_renderer, this->_bg_color.r, this->_bg_color.g, this->_bg_color.b, this->_bg_color.a);
    SDL_RenderClear(this->_renderer);
    SDL_RenderCopyEx(this->_renderer, this->_board_grid_tex, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
    SDL_RenderCopyEx(this->_renderer, this->_board_stones_tex, NULL, NULL, 0, NULL, SDL_FLIP_NONE);

    SDL_RenderPresent(this->_renderer);
}


void    GraphicalInterface::add_stone(Eigen::Array2i pos, uint8_t pid) {
    int32_t         size = 56;
    Eigen::Array2i  spos = this->grid_to_screen(pos);
    SDL_Rect        rect = {spos[1]-size/2, spos[0]-size/2, size, size};

    SDL_SetRenderTarget(this->_renderer, this->_board_stones_tex);
    SDL_RenderCopy(this->_renderer, (pid == 1 ? this->_black_stone_tex : this->_white_stone_tex), NULL, &rect);
    SDL_SetRenderTarget(this->_renderer, NULL);
}


void    GraphicalInterface::del_stone(Eigen::Array2i pos) {
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
    SDL_DestroyTexture(this->_white_stone_tex);
    SDL_DestroyTexture(this->_black_stone_tex);
    this->_white_stone_tex = NULL;
    this->_black_stone_tex = NULL;
    SDL_DestroyTexture(this->_board_stones_tex);
    SDL_DestroyTexture(this->_board_grid_tex);
    this->_board_stones_tex = NULL;
    this->_board_grid_tex = NULL;

    SDL_DestroyWindow(this->_window);
    SDL_DestroyRenderer(this->_renderer);
    this->_window = NULL;
    this->_renderer = NULL;

    IMG_Quit();
    SDL_Quit();
}
