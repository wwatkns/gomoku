#include "gomoku.hpp"
#include "Game.hpp"
#include "GraphicalInterface.hpp"

#include <bitset>

int     main(int argc, char **argv) {
    // uint64_t m = 2;
    // uint64_t v = 0b0000000000000000000000000000001000000000000000000000000000000000; // we want to find (6, 8) as x,y  34 offset
    // uint64_t p = 0;
    //
    // std::cout << std::bitset<64>(v) << " : " << v << std::endl;
    //
    // while (p++ < 64) if (0x8000000000000000 & (v << p)) break;
    // std::cout << std::bitset<64>(p) << " : " << p  << std::endl;
    //
    // std::cout << (64*m+p)/19 << std::endl; //
    // std::cout << (64*m+p)%19 << std::endl;
    //
    //
    // exit(0);

    Game    *game = new Game();

    game->loop();
    return (0);
}
