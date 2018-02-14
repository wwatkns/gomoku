#include "Game.hpp"
#include <thread> // TMP

Game::Game(void)  {
    // this->_input_handler = new InputHandler();
    // std::vector<std::string>    players = this->_input_handler.set_players();

    this->_game_engine = new GameEngine();
    this->_gui = new GraphicalInterface(this->_game_engine);

    /* Debug */
    this->_player_1 = new Human(this->_game_engine, this->_gui, 1);
    this->_player_2 = new Human(this->_game_engine, this->_gui, 2);
    // this->_player_2 = new Computer(this->_game_engine, 2);
    this->_c_player = this->_player_1;
    this->_gui->get_analytics()->set_players(this->_c_player, this->_player_1, this->_player_2);

    /* initial menu */
    this->_gui->update_events();
    this->_gui->update_display();
    this->_gui->render_choice_menu();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

Game::Game(Game const &src) {
    *this = src;
}

Game::~Game(void) {
    delete this->_player_1;
    delete this->_player_2;
    delete this->_c_player;
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
    t_action    c_action;

    while (true) {
        this->_gui->update_events();

        c_action = this->_c_player->play();

        if (this->_gui->check_close())
            break;
        this->_game_engine->update_game_state(c_action, *this->_c_player);
        if (this->_game_engine->check_end(this->_c_player->get_pair_captured()) == true)
            break;
        this->_gui->get_analytics()->set_c_player(this->_c_player->get_id() == 1 ? this->_player_2 : this->_player_1);
        this->_gui->update_display();
        this->_c_player = (this->_c_player->get_id() == 1 ? this->_player_2 : this->_player_1); /* switch */
    }
}

void        Game::end(void) const {
}
