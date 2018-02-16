#include "Game.hpp"

Game::Game(void)  {
    /*  TODO: change the undo button so that the Computer will wait a bit before putting a stone because
        (maybe undo the last actions until the last player)
    */

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

/* Getters */
Player      *Game::get_player_1(void) const { return (this->_player_1); }
Player      *Game::get_player_2(void) const { return (this->_player_2); }
GameEngine  *Game::get_game_engine(void) const { return (this->_game_engine); }
/* Setters */
void        Game::set_player_1(Player player) { this->_player_1 = &player; }
void        Game::set_player_2(Player player) { this->_player_2 = &player; }


void        Game::loop(void) {
    while (true) {
        this->_gui->update_events();
        this->_c_player->play();
        if (this->_gui->check_undo())
            if (undo())
                continue;
        if (this->_gui->check_newgame())
            newgame();
        if (this->_gui->check_restart())
            restart();
        if (this->_gui->check_close()) /* will quit the game */
            break;
        if (this->_game_engine->check_end(this->_c_player->get_pair_captured()) == true) /* will display an end message */
            break;
        this->_gui->get_analytics()->set_c_player(this->_c_player->get_id() == 1 ? this->_player_2 : this->_player_1);
        this->_gui->update_display();
        this->_c_player = (this->_c_player->get_id() == 1 ? this->_player_2 : this->_player_1); /* switch */
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

void        Game::end(void) const {
}
