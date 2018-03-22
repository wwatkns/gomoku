#include "ZobristTable.hpp"

namespace ZobristTable {

std::array<std::array<uint64_t, S>, SIZE>   _init_zobrist_table(void) {
    std::array<std::array<uint64_t, S>, SIZE> table;
    std::srand(std::time(nullptr));
    for (int n = 0; n < SIZE; n++) {
        for (int s = 0; s < S; s++)
            table[n][s] = std::rand();
    }
    return (table);
}
}