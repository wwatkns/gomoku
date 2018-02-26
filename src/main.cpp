#include "gomoku.hpp"
#include "Game.hpp"
#include "GraphicalInterface.hpp"

#include "BitBoard.hpp" // TEST

#include <bitset>

int     main(int argc, char **argv) {
    // Game                *game = new Game();
    //
    // game->loop();
    // exit(0);
    BitBoard *p1 = new BitBoard();

    // std::cout << (*p1) << std::endl;
    // p1->broadcast_row(0xCCCCA00000000000);
    // p1->broadcast_row(0x00000000000CCCCA);

    p1->values[0] = 0xFFFFE00000000000;
    p1->values[1] = 0x000FFFFE00000000;
    p1->values[2] = 0x07FFFF0000000000;
    p1->values[3] = 0x00007FFFF0000000;
    p1->values[4] = 0x003FFFF800000000;
    p1->values[5] = 0x000003FFFF800000;

    std::cout << (*p1) << std::endl;
    // p1->broadcast_row(0xAAAAA00000000000);
    // std::cout << (*p1) << std::endl;
    // p1->broadcast_row(0xCCCCC00000000000);
    // std::cout << (*p1) << std::endl;
    // p1->broadcast_row(0xFF00000000000000);
    // std::cout << (*p1) << std::endl;

    *p1 = p1->rotateRight45();
    std::cout << *p1 << std::endl;

    *p1 &= ~(*p1);
    std::cout << *p1 << std::endl;

    // p1->values[0] = 0x8000000000000000;
    // *p1 <<= 360;

    for (uint8_t i = 0; i < 6; i++)
        p1->write_move(i, i);
    std::cout << *p1 << std::endl;
    std::cout << std::to_string( detect_five_alignment(*p1) ) << std::endl;

    p1->delete_move(4, 4);
    std::cout << *p1 << std::endl;
    std::cout << std::to_string( detect_five_alignment(*p1) ) << std::endl;

    p1->delete_move(5, 5);
    std::cout << *p1 << std::endl;
    std::cout << std::to_string( detect_five_alignment(*p1) ) << std::endl;
    
    // std::cout << p1->shifted(2, 2) << std::endl;

    // std::cout << (*p1) << std::endl;

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
