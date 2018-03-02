#include "gomoku.hpp"
#include "Game.hpp"
#include "GraphicalInterface.hpp"

#include <bitset>

int     main(int argc, char **argv) {
    Game    *game = new Game();

    game->loop();
    return (0);
}
