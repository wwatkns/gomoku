#include "GraphicalInterface.hpp"

GraphicalInterface::GraphicalInterface(GameEngine *game_engine) : _game_engine(game_engine) {
    this->_quit = false;
    this->_mouse_action = false;
    this->_init_sdl();
    this->_win_res_ratio = (this->_res_h / (float)this->_win_h);
    // multiply by ratio between window size and resolution
    this->_grid_padding = (int32_t)(this->_win_h * 0.00625);    /* defaults to 8 for screen size of 1280 */
    this->_stone_size = (int32_t)(this->_res_h * 0.04375);     /* defaults to 56 for screen size of 1280 */
    this->_pad[1] = (int32_t)(this->_res_w * (float)(this->_grid_padding / 100.));
    this->_pad[0] = (int32_t)(this->_res_h * (float)(this->_grid_padding / 100.));
    this->_inc[1] = (float)(this->_res_w - (this->_pad[1] * 2)) / (COLS-1);
    this->_inc[0] = (float)(this->_res_h - (this->_pad[0] * 2)) / (ROWS-1);
    this->_bg_color = (SDL_Color){ 215, 171, 84, 255 };
    this->_load_images();
    this->_init_grid();
}

GraphicalInterface::GraphicalInterface(GraphicalInterface const &src) : _game_engine(src.get_game_engine()) {
    *this = src;
}

GraphicalInterface::~GraphicalInterface(void) {
    this->_close_sdl();
}

GraphicalInterface	&GraphicalInterface::operator=(GraphicalInterface const &src) {
    return (*this);
}

GameEngine      *GraphicalInterface::get_game_engine(void) const { return (this->_game_engine); }
Eigen::Array2i  GraphicalInterface::get_mouse_pos(void) const { return (this->_mouse_pos); }

SDL_Texture *GraphicalInterface::load_texture(std::string path) {
    IMG_Init(IMG_INIT_PNG);
    SDL_Texture *texture = NULL;
    SDL_Surface *loadedSurface = IMG_Load(path.c_str());
    texture = SDL_CreateTextureFromSurface(this->_renderer, loadedSurface);
    SDL_FreeSurface(loadedSurface);
    return texture;
}

Eigen::Array2i  GraphicalInterface::grid_to_screen(Eigen::Array2i pos) {
    Eigen::Array2i  s_pos;
    s_pos[0] = this->_pad[0] + pos[0] * this->_inc[0];
    s_pos[1] = this->_pad[1] + pos[1] * this->_inc[1];
    return s_pos;
}

Eigen::Array2i  GraphicalInterface::screen_to_grid(Eigen::Array2i pos) {
    Eigen::Array2i  g_pos;
    g_pos[0] = std::round((pos[0] - this->_pad[0]) / this->_inc[0]);
    g_pos[1] = std::round((pos[1] - this->_pad[1]) / this->_inc[1]);
    g_pos[0] = std::min(std::max(g_pos[0], 0), COLS-1);
    g_pos[1] = std::min(std::max(g_pos[1], 0), ROWS-1);
    return g_pos;
}

Eigen::Array2i  GraphicalInterface::snap_to_grid(Eigen::Array2i pos) {
    Eigen::Array2i  s_pos;
    s_pos[0] = this->_pad[0] + std::round((pos[0] - this->_pad[0]) / this->_inc[0]) * this->_inc[0];
    s_pos[1] = this->_pad[1] + std::round((pos[1] - this->_pad[1]) / this->_inc[1]) * this->_inc[1];
    s_pos[0] = std::min(std::max(s_pos[0], this->_pad[0]), this->_res_h - this->_pad[0]);
    s_pos[1] = std::min(std::max(s_pos[1], this->_pad[1]), this->_res_w - this->_pad[1]);
    return s_pos;
}

void    GraphicalInterface::_load_images(void) {
    this->_white_stone_tex = this->load_texture(std::string("./resources/circle_white.png"));
    this->_black_stone_tex = this->load_texture(std::string("./resources/circle_black.png"));
    this->_select_stone_tex = this->load_texture(std::string("./resources/circle_select.png"));
}

void    GraphicalInterface::_init_sdl(void) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    /* get the screen dimensions */
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    this->_win_w = this->_win_h = (int32_t)(std::min(DM.w, DM.h) / 1.125);

    this->_window = SDL_CreateWindow("gomoku", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->_win_w, this->_win_h, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    this->_renderer = SDL_CreateRenderer(this->_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
    this->_board_grid_tex = SDL_CreateTexture(this->_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->_win_w, this->_win_h);

    /* get the renderer resolution, may differ from window size for high-dpi displays */
    SDL_GetRendererOutputSize(this->_renderer, &this->_res_w, &this->_res_h);

    SDL_SetRenderDrawBlendMode(this->_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(this->_board_grid_tex, SDL_BLENDMODE_BLEND);
}

void    GraphicalInterface::_init_grid(void) {
    Eigen::Array2i  pad = this->_pad / (int32_t)this->_win_res_ratio;
    Eigen::Array2f  inc = this->_inc / this->_win_res_ratio;

    SDL_SetRenderTarget(this->_renderer, this->_board_grid_tex);
    SDL_RenderClear(this->_renderer);
    SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 255);
    for (int i = 0; i < ROWS; i++) {
        SDL_RenderDrawLine(this->_renderer, pad[1], (int)(pad[0]+i*inc[0]),
            this->_win_w - pad[1], (int)(pad[0]+i*inc[0]));
    }
    for (int i = 0; i < COLS; i++) {
        SDL_RenderDrawLine(this->_renderer, (int)(pad[1]+i*inc[1]), pad[0],
            (int)(pad[0]+i*inc[0]), this->_win_h - pad[0]);
    }
    this->_init_grid_points();
    SDL_SetRenderTarget(this->_renderer, NULL);
}

void    GraphicalInterface::_init_grid_points(void) {
    Eigen::Array2i  pos;
    SDL_Rect        rect;

    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < 3; i++) {
            pos = this->grid_to_screen((Eigen::Array2i){3+j*6, 3+i*6}) / (int32_t)this->_win_res_ratio;
            rect = {pos[1]-2, pos[0]-2, 5, 5};
            SDL_RenderCopy(this->_renderer, this->_black_stone_tex, NULL, &rect);
        }
    }
}

void    GraphicalInterface::update_events(void) {
    while (SDL_PollEvent(&this->_event) != 0) {
        switch (this->_event.type) {
            case SDL_QUIT: this->_quit = true; break;
            case SDL_MOUSEMOTION: SDL_GetMouseState(&this->_mouse_pos[1], &this->_mouse_pos[0]);
                this->_mouse_pos *= this->_win_res_ratio;
                break;
            case SDL_MOUSEBUTTONUP: this->_mouse_action = true; break;
        }
    }
    this->_key_states = SDL_GetKeyboardState(NULL);
    if (this->_key_states[SDL_SCANCODE_ESCAPE])
        this->_quit = true;
}

void    GraphicalInterface::update_display(void) {
    SDL_SetRenderDrawColor(this->_renderer, this->_bg_color.r, this->_bg_color.g, this->_bg_color.b, this->_bg_color.a);
    SDL_RenderClear(this->_renderer);
    SDL_RenderCopyEx(this->_renderer, this->_board_grid_tex, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
    this->_render_stones();
    this->_render_select();
    SDL_RenderPresent(this->_renderer);
}

void    GraphicalInterface::_render_stones(void) {
    /*  TODO:
        should optimize to only the update the stones that were updated, and not search the whole grid each time.
    */
    Eigen::Array2i  s_pos;
    SDL_Rect        rect;
    SDL_Texture     *stone;

    for (int j = 0; j < COLS; j++) {
        for (int i = 0; i < ROWS; i++) {
            if (this->_game_engine->grid(j,i) == -1 || this->_game_engine->grid(j,i) == 1) {
                s_pos = this->grid_to_screen((Eigen::Array2i){j,i});
                rect = {s_pos[1] - this->_stone_size / 2, s_pos[0] - this->_stone_size / 2, this->_stone_size, this->_stone_size};
                stone = (this->_game_engine->grid(j,i) == -1 ? this->_black_stone_tex : this->_white_stone_tex);
                SDL_RenderCopy(this->_renderer, stone, NULL, &rect);
            }
        }
    }
}

void    GraphicalInterface::_render_select(void) {
    Eigen::Array2i  s_pos;
    Eigen::Array2i  g_pos;
    SDL_Rect        rect;

    s_pos = this->snap_to_grid(this->_mouse_pos);
    g_pos = this->screen_to_grid(this->_mouse_pos);
    rect = {s_pos[1] - this->_stone_size / 2, s_pos[0] - this->_stone_size / 2, this->_stone_size, this->_stone_size};
    if (this->_game_engine->grid(g_pos[0], g_pos[1]) != -1 && this->_game_engine->grid(g_pos[0], g_pos[1]) != 1)
        SDL_RenderCopy(this->_renderer, this->_select_stone_tex, NULL, &rect);
}

bool    GraphicalInterface::check_mouse_action(void) {
    if (this->_mouse_action == true) {
        this->_mouse_action = false;
        return true;
    }
    return false;
}

bool    GraphicalInterface::check_close(void) {
    return this->_quit;
}

void    GraphicalInterface::_close_sdl(void) {
    SDL_DestroyTexture(this->_white_stone_tex);
    SDL_DestroyTexture(this->_black_stone_tex);
    SDL_DestroyTexture(this->_select_stone_tex);
    this->_white_stone_tex = NULL;
    this->_black_stone_tex = NULL;
    this->_select_stone_tex = NULL;
    SDL_DestroyTexture(this->_board_grid_tex);
    this->_board_grid_tex = NULL;

    SDL_DestroyWindow(this->_window);
    SDL_DestroyRenderer(this->_renderer);
    this->_window = NULL;
    this->_renderer = NULL;

    IMG_Quit();
    SDL_Quit();
}
