#include "gomoku.hpp"
#include "Game.hpp"
#include "GraphicalInterface.hpp"

#include "BitBoard.hpp" // TEST

#include <bitset>

int     main(int argc, char **argv) {
    // Game                *game = new Game();

    // game->loop();
    BitBoard *p1 = new BitBoard();

    p1->values[3] = 129;
    std::cout << (*p1) << std::endl;
    *p1 = dilation(*p1);
    *p1 = dilation(*p1);
    std::cout << (*p1) << std::endl;
    *p1 >>= 20;
    std::cout << (*p1) << std::endl;
    // std::cout << erosion(*p1) << std::endl;

    // p1->values[1] = 127;
    // std::cout << (*p1) << std::endl;
    // std::cout << (*p1 >> 32) << std::endl;
    // std::cout << (*p1 >> 63) << std::endl;
    // std::cout << (*p1 >> 127) << std::endl;
    // std::cout << (*p1 >> 128) << std::endl;
    // std::cout << (*p1 >> 193) << std::endl;

    // std::cout << (BitBoard::full_mask << 124) << std::endl;
    // std::cout << (BitBoard::full_mask) << std::endl;
    // std::cout << (BitBoard::full_mask << 104) << std::endl;
    // std::cout << (BitBoard::full_mask << 105) << std::endl;
    // std::cout << (BitBoard::full_mask << 106) << std::endl;

    // std::cout << (std::bitset<16>(2) << -1) << std::endl;
    return (0);
}
