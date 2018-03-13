#include "gomoku.hpp"
#include "Game.hpp"
#include "GraphicalInterface.hpp"

#include <bitset>
#include "BitBoard.hpp"

int     main(int argc, char **argv) {
    // uint64_t    mask = 1;
    //
    // for (int i = 0; i < 32; i++) {
    //     mask |= mask << 1;
    //     std::cout << std::bitset<64>(mask) << " : " << std::hex << mask << std::endl;
    // }
    //
    // exit(1);
    Game    *game = new Game();

    game->loop();
    return (0);
}
