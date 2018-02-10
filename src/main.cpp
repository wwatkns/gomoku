#include "gomoku.hpp"
#include "Game.hpp"
#include "GraphicalInterface.hpp"

int     main(int argc, char **argv) {
    Game                *game = new Game();

    game->loop();
    // game->end();
    return (0);
}
