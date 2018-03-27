#include "Game.hpp"

int     main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    Game    *game = new Game();

    game->loop();
    return (0);
}
