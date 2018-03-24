#include "gomoku.hpp"
#include "Game.hpp"
#include "GraphicalInterface.hpp"

#include <bitset>
#include "BitBoard.hpp"
#include <list>

// static int popcount64(uint64_t x) {
//     x = x - ((x >> 1) & 0x5555555555555555);
//     x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333);
//     return (((x + (x >> 4)) & 0x0F0F0F0F0F0F0F0F) * 0x0101010101010101) >> 56;
// }

static int popcount64(uint64_t x) {
    int res = 0;
    for (;x;res++)
        x &= x - 1;
    return (res);
}

std::list<uint16_t> move_serialization(BitBoard &moves) {
    std::list<uint16_t> serialized;
    /* convert the bitboard of moves to a list of positions */
    for (int i = 0; i < N; ++i)
        if (moves.values[i]) do {
            int idx = 63 - popcount64((moves.values[i] & -moves.values[i]) - 1) + (BITS * i);
            serialized.push_back(idx);
        } while (moves.values[i] &= moves.values[i]-1);
    return (serialized);
}


int     main(int argc, char **argv) {
    // BitBoard    test;
    //
    // test.write(360);
    // test.write(0);
    // test.write(15);
    // test.write(78);
    // test.write(16);
    // test.write(134);
    // std::cout << test << std::endl;
    //
    // std::list<uint16_t> moves = move_serialization(test);
    // for (std::list<uint16_t>::const_iterator it = moves.begin(); it != moves.end(); ++it) {
    //     std::cout << *it << std::endl;
    // }
    // exit(1);

    Game    *game = new Game();

    game->loop();
    return (0);
}
