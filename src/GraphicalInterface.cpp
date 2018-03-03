#include "GraphicalInterface.hpp"
#include "Player.hpp"

GraphicalInterface::GraphicalInterface(GameEngine *game_engine) : _game_engine(game_engine) {
    this->_quit = false;
    this->_mouse_action = false;
    this->_init_sdl();

    this->_font_handler = new FontHandler(this->_renderer, this->_res_ratio);
    this->_analytics = new Analytics(this->_game_engine, this->_font_handler, this->_res_ratio);

    this->_grid_padding = 8;
    this->_stone_size = (int32_t)(this->_res_h * 0.04375);
    this->_pad[1] = (int32_t)(this->_main_viewport.w * (float)(this->_grid_padding / 100.));
    this->_pad[0] = (int32_t)(this->_res_h * (float)(this->_grid_padding / 100.));
    this->_inc[1] = (float)(this->_main_viewport.w - (this->_pad[1] * 2)) / (COLS-1);
    this->_inc[0] = (float)(this->_res_h - (this->_pad[0] * 2)) / (ROWS-1);
    this->_load_images();
    this->_init_grid();

    TTF_Font *font = this->_font_handler->load_font("./resources/fonts/Montserrat-Regular.ttf", (int32_t)(14 * this->_res_ratio));
    Eigen::Array2i  button_padding = this->_handle_ratio((Eigen::Array2i){ 12, 5 });
    this->_button_newgame = new Button(this->_renderer, "new game", {this->_main_viewport.w + (int32_t)(10 * this->_res_ratio), (int32_t)(320 * this->_res_ratio)}, button_padding, font, this->_color_win, this->_color_font_2, this->_color_onhover, this->_color_outline);
    this->_button_restart = new Button(this->_renderer, "restart", {this->_main_viewport.w + (int32_t)(10 * this->_res_ratio), (int32_t)(350 * this->_res_ratio)}, button_padding, font, this->_color_win, this->_color_font_2, this->_color_onhover, this->_color_outline);
    this->_button_pause = new ButtonSwitch(this->_renderer, "pause", "resume", {this->_main_viewport.w + (int32_t)(10 * this->_res_ratio), (int32_t)(380 * this->_res_ratio)}, button_padding, font, this->_color_win, this->_color_font_2, this->_color_onhover, this->_color_outline);
    this->_button_undo = new Button(this->_renderer, "undo", {this->_main_viewport.w + (int32_t)(10 * this->_res_ratio), (int32_t)(410 * this->_res_ratio)}, button_padding, font, this->_color_win, this->_color_gold, this->_color_onhover, this->_color_gold);

    this->_winning_font = this->_font_handler->load_font("./resources/fonts/Montserrat-Regular.ttf", (int32_t)(24 * this->_res_ratio));
    this->_winning_text = "";
    this->_winning_color = {255, 255, 255, 255};
    this->_winning_font_text = new FontText(&this->_winning_text, {this->_main_viewport.w/2, this->_main_viewport.h/2}, "center", "center", this->_winning_font, &this->_winning_color, this->_renderer);
}

GraphicalInterface::GraphicalInterface(GraphicalInterface const &src) : _game_engine(src.get_game_engine()) {
    *this = src;
}

GraphicalInterface::~GraphicalInterface(void) {
    delete this->_font_handler;
    delete this->_analytics;
    delete this->_button_newgame;
    delete this->_button_restart;
    delete this->_button_pause;
    delete this->_button_undo;
    this->_close_sdl();
}

GraphicalInterface	&GraphicalInterface::operator=(GraphicalInterface const &src) {
    return (*this);
}

SDL_Texture *GraphicalInterface::load_texture(std::string path) {
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
    s_pos[0] = std::min(std::max(s_pos[0], this->_pad[0]), this->_main_viewport.h - this->_pad[0]);
    s_pos[1] = std::min(std::max(s_pos[1], this->_pad[1]), this->_main_viewport.w - this->_pad[1]);
    return s_pos;
}

void    GraphicalInterface::_load_images(void) {
    this->_white_tex = this->load_texture("./resources/circle_white.png");
    this->_white_stone_tex = this->load_texture("./resources/circle_white_outlined.png");
    this->_black_stone_tex = this->load_texture("./resources/circle_black.png");
    this->_select_stone_tex = this->load_texture("./resources/circle_select.png");
}

void    GraphicalInterface::_init_sdl(void) {
    int32_t secondary_viewport_width = 300;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");
    IMG_Init(IMG_INIT_PNG);

    /* get the screen dimensions */
    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    this->_win_h = (int32_t)(std::min(DM.w, DM.h) / 1.125);
    this->_win_w = this->_win_h + secondary_viewport_width;

    this->_window = SDL_CreateWindow("gomoku", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, this->_win_w, this->_win_h, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    this->_renderer = SDL_CreateRenderer(this->_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);

    /* get the renderer resolution, may differ from window size for high-dpi displays */
    SDL_GetRendererOutputSize(this->_renderer, &this->_res_w, &this->_res_h);
    this->_res_ratio = (this->_res_h / (float)this->_win_h);
    secondary_viewport_width = (int32_t)(secondary_viewport_width * this->_res_ratio);
    this->_global_viewport = (SDL_Rect){ 0, 0, this->_res_w, this->_res_h };
    this->_main_viewport = (SDL_Rect){ 0, 0, (int32_t)(this->_res_w - secondary_viewport_width), this->_res_h };
    this->_secondary_viewport = (SDL_Rect){ (int32_t)(this->_res_w - secondary_viewport_width), 0, secondary_viewport_width, this->_res_h };

    this->_board_grid_tex = SDL_CreateTexture(this->_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, this->_main_viewport.w, this->_main_viewport.h);

    SDL_SetRenderDrawBlendMode(this->_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetTextureBlendMode(this->_board_grid_tex, SDL_BLENDMODE_BLEND);
}

void    GraphicalInterface::_init_grid(void) {
    SDL_SetRenderTarget(this->_renderer, this->_board_grid_tex);
    SDL_RenderClear(this->_renderer);
    SDL_SetRenderDrawColor(this->_renderer, _color_board_grid.r, _color_board_grid.b, _color_board_grid.g, _color_board_grid.a);
    for (int i = 0; i < ROWS; i++) {
        SDL_RenderDrawLine(this->_renderer, this->_pad[1], (int)(this->_pad[0]+i*this->_inc[0]),
            this->_main_viewport.w - this->_pad[1], (int)(this->_pad[0]+i*this->_inc[0]));
    }
    for (int i = 0; i < COLS; i++) {
        SDL_RenderDrawLine(this->_renderer, (int)(this->_pad[1]+i*this->_inc[1]), this->_pad[0],
            (int)(this->_pad[0]+i*this->_inc[0]), this->_res_h - this->_pad[0]);
    }
    this->_init_grid_points();
    this->_init_grid_indicators();
    this->_init_forbidden();
    /* secondary viewport boundary */
    SDL_Rect    rect = { this->_main_viewport.w - 2, 0, 2, this->_main_viewport.h };
    SDL_SetRenderDrawColor(this->_renderer, this->_color_black.r, this->_color_black.g, this->_color_black.b, this->_color_black.a);
    SDL_RenderFillRect(this->_renderer, &rect);
    SDL_SetRenderTarget(this->_renderer, NULL);
}

void    GraphicalInterface::_init_grid_points(void) {
    Eigen::Array2i  pos;
    SDL_Rect        rect;
    int32_t         pt_size = (int32_t)(0.005625 * this->_main_viewport.w);

    for (int j = 0; j < 3; j++) {
        for (int i = 0; i < 3; i++) {
            pos = this->grid_to_screen((Eigen::Array2i){3+j*6, 3+i*6});
            rect = {pos[1]-(pt_size/2), pos[0]-(pt_size/2), pt_size, pt_size};
            SDL_RenderCopy(this->_renderer, this->_black_stone_tex, NULL, &rect);
        }
    }
}

void    GraphicalInterface::_init_grid_indicators(void) {
    TTF_Font        *font = this->_font_handler->load_font("./resources/fonts/Montserrat-Regular.ttf", (int32_t)(14 * this->_res_ratio));
    FontText        *font_text;
    Eigen::Array2i  pos;
    std::string     text;

    for (uint8_t j = 0; j < 2; j++) {
        for (uint8_t i = 0; i < ROWS; i++) {
            text = std::to_string(ROWS-i);
            pos = { (j ? this->_main_viewport.w-this->_pad[1]/2 : this->_pad[1]/2), (int32_t)(this->_pad[0]+i*this->_inc[0]) };
            font_text = new FontText(&text, pos, "center", "center", font, &this->_color_board_grid, this->_renderer);
            font_text->render_text();
            delete font_text;
        }
        for (uint8_t i = 0; i < COLS; i++) {
            text = std::string(1, "ABCDEFGHJKLMNOPQRST"[i]);
            pos = { (int32_t)(this->_pad[1]+i*this->_inc[1]), (j ? this->_main_viewport.h-this->_pad[0]/2 : this->_pad[0]/2) };
            font_text = new FontText(&text, pos, "center", "center", font, &this->_color_board_grid, this->_renderer);
            font_text->render_text();
            delete font_text;
        }
    }
}

void    GraphicalInterface::_init_forbidden(void) {
    std::string  text = "F";
    TTF_Font     *font = this->_font_handler->load_font("./resources/fonts/Montserrat-Regular.ttf", (int32_t)(this->_res_h * 0.015625));
    SDL_Surface  *surf = TTF_RenderText_Shaded(font, text.c_str(), this->_color_black, this->_color_board_bg);

    this->_forbidden_rect = {0, 0, 0, 0};
    this->_forbidden_tex = SDL_CreateTextureFromSurface(this->_renderer, surf);
    SDL_FreeSurface(surf);
    TTF_SizeText(font, text.c_str(), &this->_forbidden_rect.w, &this->_forbidden_rect.h);
}

void    GraphicalInterface::update_events(void) {
    this->_mouse_action = false;
    while (SDL_PollEvent(&this->_event) != 0) {
        switch (this->_event.type) {
            case SDL_QUIT: this->_quit = true; break;
            case SDL_MOUSEMOTION: SDL_GetMouseState(&this->_mouse_pos[1], &this->_mouse_pos[0]);
                this->_mouse_pos *= this->_res_ratio;
                break;
            case SDL_MOUSEBUTTONUP: this->_mouse_action = true; break;
        }
    }
    this->_key_states = SDL_GetKeyboardState(NULL);
    if (this->_key_states[SDL_SCANCODE_ESCAPE])
        this->_quit = true;
    /* update the buttons states */
    this->_button_newgame->update_state(&this->_mouse_pos, this->_mouse_action);
    this->_button_restart->update_state(&this->_mouse_pos, this->_mouse_action);
    this->_button_pause->update_state(&this->_mouse_pos, this->_mouse_action);
    this->_button_undo->update_state(&this->_mouse_pos, this->_mouse_action);
}

void    GraphicalInterface::update_display(void) {
    this->_render_board();
    this->_render_stones();
    this->_render_select();
    this->_render_secondary_viewport();
    this->_render_buttons();
    this->_render_pause();
    this->_render_winning_screen();
    SDL_RenderPresent(this->_renderer);
}

void    GraphicalInterface::update_end_game(Player const &p1, Player const &p2) {
    // if (this->_game_engine->check_end(player->get_pairs_captured()) == 1) { // TODO : implement draw condition (check_end() == 2)
    if (this->_game_engine->check_end(p1, p2) == 1) { // TODO : implement draw condition (check_end() == 2)
        std::string type = (p1.type == 0 ? "human" : "computer");
        this->_winning_color = (p1.type == 0 ? (SDL_Color){255, 255, 255, 255} : (SDL_Color){219, 15, 59, 255});
        this->_winning_text = std::string("Player ")+std::to_string(p1.get_id())+std::string(" (")+type+std::string(") wins");
        this->_button_pause->set_state(true);
        this->_end_game = true;
    } else {
        this->_winning_text = "";
        if (this->_end_game)
            this->_button_pause->set_state(false);
        this->_end_game = false;
    }
}

void    GraphicalInterface::_render_board(void) {
    SDL_RenderSetViewport(this->_renderer, &this->_main_viewport);
    SDL_SetRenderDrawColor(this->_renderer, this->_color_board_bg.r, this->_color_board_bg.g, this->_color_board_bg.b, this->_color_board_bg.a);
    SDL_RenderClear(this->_renderer);
    SDL_RenderCopyEx(this->_renderer, this->_board_grid_tex, NULL, NULL, 0, NULL, SDL_FLIP_NONE);
}

void    GraphicalInterface::_render_stones(void) {
    Eigen::Array2i  s_pos;
    SDL_Rect        rect;
    Eigen::Array2i  size;
    SDL_Texture     *stone;

    for (int j = 0; j < COLS; j++) {
        for (int i = 0; i < ROWS; i++) {
            /* stones */
            if (this->_game_engine->grid(j,i) == -1 || this->_game_engine->grid(j,i) == 1) { /* TODO: more modular, change 1 and -1 */
                s_pos = this->grid_to_screen((Eigen::Array2i){j,i});
                rect = {s_pos[1] - this->_stone_size / 2, s_pos[0] - this->_stone_size / 2, this->_stone_size, this->_stone_size};
                stone = (this->_game_engine->grid(j,i) == -1 ? this->_black_stone_tex : this->_white_stone_tex);
                SDL_RenderCopy(this->_renderer, stone, NULL, &rect);
            }
            /* forbidden */
            // if (this->_game_engine->grid(j,i) == -10 || this->_game_engine->grid(j,i) == 10) { /* show all forbiddens */
            if (this->_game_engine->grid(j,i) == (this->_analytics->get_c_player()->get_id()==1?-1:1)*10) { /* show current player forbiddens */
                s_pos = this->grid_to_screen((Eigen::Array2i){j,i});
                /* rect */
                size = { this->_forbidden_rect.w*3, this->_forbidden_rect.h*1.5 };
                rect = { s_pos[1]-size[0]/2, s_pos[0]-size[1]/2, size[0], size[1] };
                SDL_SetRenderDrawColor(this->_renderer, this->_color_board_bg.r, this->_color_board_bg.g, this->_color_board_bg.b, this->_color_board_bg.a);
                SDL_RenderFillRect(this->_renderer, &rect);
                this->_forbidden_rect.x = s_pos[1]-this->_forbidden_rect.w/2;
                this->_forbidden_rect.y = s_pos[0]-this->_forbidden_rect.h/2;
                /* text */
                SDL_RenderCopy(this->_renderer, this->_forbidden_tex, NULL, &this->_forbidden_rect);
            }
        }
    }
    /* look for the last action and display an indicator on it */
    if (this->_game_engine->get_history()->size() > 0 and !this->_end_game) {
        t_action    last = this->_game_engine->get_history()->back();
        int32_t     size = (int32_t)(this->_stone_size * 0.1);

        s_pos = this->grid_to_screen(last.pos);
        rect = { s_pos[1]-size/2, s_pos[0]-size/2, size, size };
        stone = (this->_game_engine->grid(last.pos[0],last.pos[1]) == 1 ? this->_black_stone_tex : this->_white_tex);
        SDL_RenderCopy(this->_renderer, stone, NULL, &rect);
    }
}

void    GraphicalInterface::_render_select(void) {
    Eigen::Array2i  s_pos;
    Eigen::Array2i  g_pos;
    SDL_Rect        rect;

    if (this->check_mouse_on_board() && !this->check_pause()) {
        s_pos = this->snap_to_grid(this->_mouse_pos);
        g_pos = this->screen_to_grid(this->_mouse_pos);
        if (this->_game_engine->grid(g_pos[0], g_pos[1]) == (this->_analytics->get_c_player()->get_id()==1?-1:1)*10) /* don't display select circle on forbidden */
            return;
        rect = {s_pos[1] - this->_stone_size / 2, s_pos[0] - this->_stone_size / 2, this->_stone_size, this->_stone_size};
        if (this->_game_engine->grid(g_pos[0], g_pos[1]) != -1 && this->_game_engine->grid(g_pos[0], g_pos[1]) != 1)
            SDL_RenderCopy(this->_renderer, this->_select_stone_tex, NULL, &rect);
    }
}

void    GraphicalInterface::_render_secondary_viewport(void) {
    SDL_RenderSetViewport(this->_renderer, &this->_secondary_viewport);
    /* draw background */
    SDL_Rect    rect = {0, 0, this->_secondary_viewport.w, this->_secondary_viewport.h};
    SDL_SetRenderDrawColor(this->_renderer, this->_color_win.r, this->_color_win.g, this->_color_win.b, this->_color_win.a);
    SDL_RenderFillRect(this->_renderer, &rect);
    /* draw lines */
    SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 255);
    int8_t width = (int8_t)(1 * this->_res_ratio);
    rect = { 0, (int32_t)( 90 * this->_res_ratio)-(width-1), this->_secondary_viewport.w, width };
    SDL_RenderFillRect(this->_renderer, &rect);
    rect = { 0, (int32_t)(200 * this->_res_ratio)-(width-1), this->_secondary_viewport.w, width }; // 180
    SDL_RenderFillRect(this->_renderer, &rect);
    rect = { 0, (int32_t)(310 * this->_res_ratio)-(width-1), this->_secondary_viewport.w, width }; // 270
    SDL_RenderFillRect(this->_renderer, &rect);
    /* draw depth line */
    SDL_SetRenderDrawColor(this->_renderer, this->_color_outline.r, this->_color_outline.g, this->_color_outline.b, this->_color_outline.a);
    rect = { 0, (int32_t)( 90 * this->_res_ratio)+1, this->_secondary_viewport.w, 1 };
    SDL_RenderFillRect(this->_renderer, &rect);
    rect = { 0, (int32_t)(200 * this->_res_ratio)+1, this->_secondary_viewport.w, 1 };
    SDL_RenderFillRect(this->_renderer, &rect);
    rect = { 0, (int32_t)(310 * this->_res_ratio)+1, this->_secondary_viewport.w, 1 };
    SDL_RenderFillRect(this->_renderer, &rect);

    this->_analytics->render_text();
}

void    GraphicalInterface::_render_buttons(void) {
    SDL_RenderSetViewport(this->_renderer, &this->_global_viewport);
    this->_button_newgame->render(this->_renderer, &this->_mouse_pos);
    this->_button_restart->render(this->_renderer, &this->_mouse_pos);
    this->_button_pause->render(this->_renderer, &this->_mouse_pos);
    this->_button_undo->render(this->_renderer, &this->_mouse_pos);
}

void    GraphicalInterface::_render_pause(void) {
    if (this->check_pause()) {
        if (this->_analytics->get_chronometer()->is_running() == true)
            this->_analytics->get_chronometer()->stop();
        SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 150);
        SDL_RenderFillRect(this->_renderer, &this->_main_viewport);
    }
    if (!this->check_pause() and this->_analytics->get_chronometer()->is_running() == false)
        this->_analytics->get_chronometer()->resume();
}

void    GraphicalInterface::_render_winning_screen(void) {
    if (this->_end_game) {
        /* draw line alignment */
        Eigen::Array22i line = this->_game_engine->get_end_line(this->_analytics->get_c_player()->board);
        Eigen::Array2i  p1 = this->grid_to_screen(line.row(0));
        Eigen::Array2i  p2 = this->grid_to_screen(line.row(1));
        SDL_Color       color = (this->_analytics->get_c_player()->get_id() == 1 ? this->_color_white : this->_color_black);
        SDL_SetRenderDrawColor(this->_renderer, color.r, color.g, color.b, color.a);
        SDL_RenderDrawLine(this->_renderer, p1(0), p1(1), p2(0), p2(1));
        /* draw stones indicators */
        if (!(p1(0) == p2(0) && p1(1) == p2(1))) {
            int32_t     size = (int32_t)(this->_stone_size * 0.1);
            SDL_Rect    rect2 = { p1(0)-size/2, p1(1)-size/2, size, size };
            SDL_RenderCopy(this->_renderer, (this->_game_engine->grid(line(0,1),line(0,0)) == 1 ? this->_black_stone_tex : this->_white_tex), NULL, &rect2);
            rect2 = { p2(0)-size/2, p2(1)-size/2, size, size };
            SDL_RenderCopy(this->_renderer, (this->_game_engine->grid(line(0,1),line(0,0)) == 1 ? this->_black_stone_tex : this->_white_tex), NULL, &rect2);
        }
        /* draw winning text */
        SDL_Rect    rect = { this->_winning_font_text->get_pos()[0], this->_winning_font_text->get_pos()[1], 0, 0 };
        TTF_SizeText(this->_winning_font_text->get_font(), this->_winning_font_text->get_text()->c_str(), &rect.w, &rect.h);
        rect.w += 20;
        rect.h += 6;
        rect.x -= rect.w / 2;
        rect.y -= rect.h / 2;
        SDL_SetRenderDrawColor(this->_renderer, 0, 0, 0, 100);
        SDL_RenderFillRect(this->_renderer, &rect);
        SDL_SetRenderDrawColor(this->_renderer, this->_winning_color.r, this->_winning_color.g, this->_winning_color.b, this->_winning_color.a);
        SDL_RenderDrawRect(this->_renderer, &rect);
        this->_winning_font_text->render_realtime_text();
    }
}

std::string GraphicalInterface::render_choice_menu(void) {
    TTF_Font    *font = this->_font_handler->load_font("./resources/fonts/Montserrat-Light.ttf", (int32_t)(14 * this->_res_ratio));
    TTF_Font    *font_bold = this->_font_handler->load_font("./resources/fonts/Montserrat-Regular.ttf", (int32_t)(16 * this->_res_ratio));
    SDL_RenderSetViewport(this->_renderer, &this->_global_viewport);

    Eigen::Array2i  button_padding = this->_handle_ratio((Eigen::Array2i){ 12, 5 });

    Button *p1_human = new Button(this->_renderer, "human", {0,0}, button_padding, font, this->_color_win, this->_color_font, this->_color_onhover, this->_color_outline);
    Button *p1_computer = new Button(this->_renderer, "computer", {0,0}, button_padding, font, this->_color_win, this->_color_font, this->_color_onhover, this->_color_outline);
    this->_menu_button_player_1 = new ButtonSelect({p1_human, p1_computer}, this->_handle_ratio((Eigen::Array2i){this->_win_w/2-82/3, this->_win_h/2-40}), 0, 'h', true);

    Button *p2_human = new Button(this->_renderer, "human", {0,0}, button_padding, font, this->_color_win, this->_color_font, this->_color_onhover, this->_color_outline);
    Button *p2_computer = new Button(this->_renderer, "computer", {0,0}, button_padding, font, this->_color_win, this->_color_font, this->_color_onhover, this->_color_outline);
    this->_menu_button_player_2 = new ButtonSelect({p2_human, p2_computer}, this->_handle_ratio((Eigen::Array2i){this->_win_w/2-82/3, this->_win_h/2-40 + 29}), 0, 'h', true);

    Button *p1 = new Button(this->_renderer, "Player 1:", this->_handle_ratio((Eigen::Array2i){this->_win_w/2-82-50, this->_win_h/2-40}), button_padding, font, this->_color_win, this->_color_font, this->_color_white, this->_color_win);
    Button *p2 = new Button(this->_renderer, "Player 2:", this->_handle_ratio((Eigen::Array2i){this->_win_w/2-82-50, this->_win_h/2-40+29}), button_padding, font, this->_color_win, this->_color_font, this->_color_white, this->_color_win);
    Button *go = new Button(this->_renderer, "Start", this->_handle_ratio((Eigen::Array2i){this->_win_w/2+108, this->_win_h/2+70}), button_padding, font, this->_color_win, this->_color_gold, this->_color_onhover, this->_color_gold);
    Button *ng = new Button(this->_renderer, "New Game", this->_handle_ratio((Eigen::Array2i){this->_win_w/2-155, this->_win_h/2-93}), button_padding, font, this->_color_header, this->_color_font, this->_color_white, this->_color_header);

    SDL_Rect    rect;
    std::string out = "";
    while (true) {
        SDL_SetRenderDrawColor(this->_renderer, this->_color_bg.r, this->_color_bg.g, this->_color_bg.b, this->_color_bg.a);
        SDL_RenderClear(this->_renderer);
        /* box */
        rect = this->_handle_ratio((SDL_Rect){ this->_win_w/2-160, this->_win_h/2-100, 320, 200 });
        SDL_SetRenderDrawColor(this->_renderer, this->_color_win.r, this->_color_win.g, this->_color_win.b, this->_color_win.a);
        SDL_RenderFillRect(this->_renderer, &rect);
        /* box header */
        rect = this->_handle_ratio((SDL_Rect){ this->_win_w/2-160, this->_win_h/2-100, 320, 38 });
        SDL_SetRenderDrawColor(this->_renderer, this->_color_header.r, this->_color_header.g, this->_color_header.b, this->_color_header.a);
        SDL_RenderFillRect(this->_renderer, &rect);
        /* box outline */
        rect = this->_handle_ratio((SDL_Rect){ this->_win_w/2-160, this->_win_h/2-100, 320, 200 });
        SDL_SetRenderDrawColor(this->_renderer, this->_color_outline.r, this->_color_outline.g, this->_color_outline.b, this->_color_outline.a);
        SDL_RenderDrawRect(this->_renderer, &rect);

        this->update_events();
        if (this->check_close())
            break;
        this->_menu_button_player_1->update_state(&this->_mouse_pos, this->_mouse_action);
        this->_menu_button_player_2->update_state(&this->_mouse_pos, this->_mouse_action);
        go->update_state(&this->_mouse_pos, this->_mouse_action);

        this->_menu_button_player_1->render(this->_renderer, &this->_mouse_pos);
        this->_menu_button_player_2->render(this->_renderer, &this->_mouse_pos);
        ng->render(this->_renderer, &this->_mouse_pos);
        go->render(this->_renderer, &this->_mouse_pos);
        p1->render(this->_renderer, &this->_mouse_pos);
        p2->render(this->_renderer, &this->_mouse_pos);

        if (go->get_state() == true) {
            out += std::string( "p1=")+std::string((this->_menu_button_player_1->get_activated_button() == 0?"H":"C"));
            out += std::string(",p2=")+std::string((this->_menu_button_player_2->get_activated_button() == 0?"H":"C"));
            break;
        }
        SDL_RenderPresent(this->_renderer);
    }
    delete p1;
    delete p2;
    delete ng;
    delete go;
    delete p1_human;
    delete p1_computer;
    delete p2_human;
    delete p2_computer;

    return out;
}

Eigen::Array2i  GraphicalInterface::_handle_ratio(Eigen::Array2i pos) {
    return {(int32_t)(pos[0] * this->_res_ratio), (int32_t)(pos[1] * this->_res_ratio)};
}

SDL_Rect        GraphicalInterface::_handle_ratio(SDL_Rect rect) {
    return {(int32_t)(rect.x*this->_res_ratio), (int32_t)(rect.y*this->_res_ratio),
            (int32_t)(rect.w*this->_res_ratio), (int32_t)(rect.h*this->_res_ratio)};
}

bool    GraphicalInterface::check_mouse_on_board(void) {
    if (this->_main_viewport.x < this->_mouse_pos[0] && this->_mouse_pos[0] < this->_main_viewport.w-1 &&
        this->_main_viewport.y < this->_mouse_pos[1] && this->_mouse_pos[1] < this->_main_viewport.h-1)
        return true;
    return false;
}

void    GraphicalInterface::_close_sdl(void) {
    SDL_DestroyTexture(this->_white_tex);
    SDL_DestroyTexture(this->_white_stone_tex);
    SDL_DestroyTexture(this->_black_stone_tex);
    SDL_DestroyTexture(this->_select_stone_tex);
    SDL_DestroyTexture(this->_forbidden_tex);
    SDL_DestroyTexture(this->_board_grid_tex);
    this->_white_tex = NULL;
    this->_white_stone_tex = NULL;
    this->_black_stone_tex = NULL;
    this->_select_stone_tex = NULL;
    this->_forbidden_tex = NULL;
    this->_board_grid_tex = NULL;

    SDL_DestroyWindow(this->_window);
    SDL_DestroyRenderer(this->_renderer);
    this->_window = NULL;
    this->_renderer = NULL;

    IMG_Quit();
    SDL_Quit();
}
