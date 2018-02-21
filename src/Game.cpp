#include "Game.hpp"

Game::Game(void)  {
    this->_game_engine = new GameEngine();
    this->_gui = new GraphicalInterface(this->_game_engine);
    /* start menu */
    this->_config = this->_gui->render_choice_menu();

    this->_player_1 = (this->_config[this->_config.find("p1=")+3]=='H' ? (Player*)new Human(this->_game_engine, this->_gui, 1) : (Player*)new Computer(this->_game_engine, 1));
    this->_player_2 = (this->_config[this->_config.find("p2=")+3]=='H' ? (Player*)new Human(this->_game_engine, this->_gui, 2) : (Player*)new Computer(this->_game_engine, 2));
    this->_c_player = this->_player_1;
    this->_gui->get_analytics()->set_players(this->_c_player, this->_player_1, this->_player_2);
}

Game::Game(Game const &src) {
    *this = src;
}

Game::~Game(void) {
    delete this->_player_1;
    delete this->_player_2;
    delete this->_game_engine;
    delete this->_gui;
}

Game	&Game::operator=(Game const &src) {
    this->_player_1 = src.get_player_1();
    this->_player_2 = src.get_player_2();
    this->_game_engine = src.get_game_engine();
    return (*this);
}

void        Game::_handle_fps(uint32_t *frames, uint32_t *ms) {
    (*frames)++;
    if (this->_gui->get_analytics()->get_chronometer()->get_elapsed_ms() - *ms >= 1000) {
        std::cout << "fps : " << *frames << std::endl;
        *ms = this->_gui->get_analytics()->get_chronometer()->get_elapsed_ms();
        *frames = 0;
    }
}

void        Game::loop(void) {
    bool        action_performed;
    bool        action_undo;
    uint32_t    ms = this->_gui->get_analytics()->get_chronometer()->get_elapsed_ms();
    uint32_t    frames = 0;

    while (true) {
        action_undo = false;
        action_performed = false;
        this->_gui->update_events();
        if (this->_gui->check_pause() == false)
            action_performed = this->_c_player->play();
        if (this->_gui->check_undo()) {
            action_undo = true;
            if (undo()) continue;
        }
        if (this->_gui->check_newgame())
            newgame();
        if (this->_gui->check_restart())
            restart();
        if (this->_gui->check_close())
            break;
        if (action_undo == false)
            this->_gui->update_end_game(this->_c_player);
        if ((action_performed == true && !this->_gui->check_pause()) || (action_undo == true && !this->_gui->get_end_game()))
            this->_c_player = (this->_c_player->get_id() == 1 ? this->_player_2 : this->_player_1); /* switch players */
        this->_gui->get_analytics()->set_c_player(this->_c_player);
        this->_gui->update_display();
        // this->_handle_fps(&frames, &ms);
    }
}

bool        Game::undo(void) {
    bool    last = (this->_game_engine->get_history_size() == 0);
    this->_game_engine->delete_last_action();
    return last;
}

/*  Restart only reset the game to 0 with the same configuration, while
    New Game displays the new game menu to configure the game.
*/
void        Game::restart(void) {
    delete this->_player_1;
    delete this->_player_2;
    delete this->_game_engine;
    delete this->_gui;

    this->_game_engine = new GameEngine();
    this->_gui = new GraphicalInterface(this->_game_engine);

    this->_player_1 = (this->_config[this->_config.find("p1=")+3]=='H' ? (Player*)new Human(this->_game_engine, this->_gui, 1) : (Player*)new Computer(this->_game_engine, 1));
    this->_player_2 = (this->_config[this->_config.find("p2=")+3]=='H' ? (Player*)new Human(this->_game_engine, this->_gui, 2) : (Player*)new Computer(this->_game_engine, 2));
    this->_c_player = this->_player_1;
    this->_gui->get_analytics()->set_players(this->_c_player, this->_player_1, this->_player_2);
    loop();
}

void        Game::newgame(void) {
    delete this->_player_1;
    delete this->_player_2;
    delete this->_game_engine;
    delete this->_gui;

    this->_game_engine = new GameEngine();
    this->_gui = new GraphicalInterface(this->_game_engine);

    this->_config = this->_gui->render_choice_menu();

    this->_player_1 = (this->_config[this->_config.find("p1=")+3]=='H' ? (Player*)new Human(this->_game_engine, this->_gui, 1) : (Player*)new Computer(this->_game_engine, 1));
    this->_player_2 = (this->_config[this->_config.find("p2=")+3]=='H' ? (Player*)new Human(this->_game_engine, this->_gui, 2) : (Player*)new Computer(this->_game_engine, 2));
    this->_c_player = this->_player_1;
    this->_gui->get_analytics()->set_players(this->_c_player, this->_player_1, this->_player_2);
    loop();
}
