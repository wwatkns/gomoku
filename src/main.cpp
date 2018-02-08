#include "gomoku.hpp"
#include "Game.hpp"
#include "GraphicalInterface.hpp"

#include <SDL.h>

int     main(int argc, char **argv) {
    Game    *game = new Game();
    GraphicalInterface  *gui = new GraphicalInterface();

    // SDL_Init(SDL_INIT_VIDEO);
    // SDL_Window*     gWindow = NULL;
    // SDL_Surface*    gScreenSurface = NULL;
    // SDL_Renderer*   gRenderer = NULL;
    // SDL_Event       e;
    //
    // gWindow = SDL_CreateWindow("Gomoku", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    // gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    // SDL_SetRenderDrawColor(gRenderer, 215, 171, 84, 255);
    //
    // // gScreenSurface = SDL_GetWindowSurface(gWindow);
    //
    // bool quit = false;
    // while (!quit) {
    //     while(SDL_PollEvent(&e) != 0) {
    //         if(e.type == SDL_QUIT)
    //             quit = true;
    //     }
    //     SDL_SetRenderDrawColor(gRenderer, 215, 171, 84, 255);
    //     SDL_RenderClear(gRenderer);
    //     // SDL_UpdateWindowSurface(gWindow);
    //
    //     // SDL_Rect fillRect = { 480 / 4, 640 / 4, 480 / 2, 640 / 2 };
    //     // SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
    //     // SDL_RenderFillRect(gRenderer, &fillRect);
    //
    //     int padding = 10;
    //     for (int i = 0; i < 19; i++) {
    //         SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
    //         SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);
    //     }
    //
    //     SDL_RenderPresent(gRenderer);
    // }
    //
    // /* CLOSE */
    // SDL_DestroyWindow(gWindow);
    // gWindow = NULL;
    // SDL_Quit();

    while (!gui->check_close()) {
        gui->update_display();
    }

    // game->loop();
    // game->end();
    return (0);
}
