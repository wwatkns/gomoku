#include "gomoku.hpp"
#include "Game.hpp"

int     main(int argc, char **argv) {
    Game    *game = new Game();

    // std::cout << game->get_game_engine()->get_initial_timestamp() << std::endl;
    game->loop();
    // game->end();
    return (0);
}
