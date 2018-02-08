#include "gomoku.hpp"
#include "Game.hpp"
#include "GraphicalInterface.hpp"

#include <SDL.h>

int     main(int argc, char **argv) {
    Game                *game = new Game();

    // this wll go inside the game loop which will have an instance of the graphicalInterface
    // while (!gui->check_close()) {
    //     gui->update_display();
    // }

    game->loop();
    // game->end();
    return (0);
}
