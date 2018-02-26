#include "gomoku.hpp"
#include "Game.hpp"
#include "GraphicalInterface.hpp"

#include "BitBoard.hpp" // TEST

#include <bitset>

int     main(int argc, char **argv) {
    // Game                *game = new Game();

    // game->loop();
    BitBoard *p1 = new BitBoard();

    // std::cout << (*p1) << std::endl;
    // p1->broadcastRow(0xCCCCA00000000000);
    // p1->broadcastRow(0x00000000000CCCCA);

    std::cout << (*p1) << std::endl;
    p1->broadcastRow(0xAAAAA00000000000);
    std::cout << (*p1) << std::endl;
    p1->broadcastRow(0xCCCCC00000000000);
    std::cout << (*p1) << std::endl;
    p1->broadcastRow(0xFF00000000000000);
    std::cout << (*p1) << std::endl;

    p1->rotateRight45();
    std::cout << (*p1) << std::endl;

    // p1->values[3] = 129;
    // std::cout << (*p1) << std::endl;
    // *p1 = dilation(*p1);
    // *p1 = dilation(*p1);
    // std::cout << (*p1) << std::endl;
    // *p1 >>= 21;
    // std::cout << (*p1) << std::endl;
    // std::cout << get_neighbours(*p1) << std::endl;

    // p1->values[1] = 127;
    // std::cout << (*p1) << std::endl;
    // std::cout << (*p1 >> 32) << std::endl;
    // std::cout << (*p1 >> 63) << std::endl;
    // std::cout << (*p1 >> 127) << std::endl;
    // std::cout << (*p1 >> 128) << std::endl;
    // std::cout << (*p1 >> 193) << std::endl;

    // std::cout << (BitBoard::full << 124) << std::endl;
    // std::cout << (BitBoard::full) << std::endl;
    // std::cout << (BitBoard::full << 104) << std::endl;
    // std::cout << (BitBoard::full << 105) << std::endl;
    // std::cout << (BitBoard::full << 106) << std::endl;

    // std::cout << (std::bitset<16>(2) << -1) << std::endl;
    return (0);
}
