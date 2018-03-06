#ifndef ZOBRISTTABLE_HPP
# define ZOBRISTTABLE_HPP

# include <iostream>
# include <cstdlib>
# include <thread>
# include <array>
# include <unordered_map>
#include "BitBoard.hpp"

# define SIZE 361   // the number of cells on the board (19*19)
# define S 3        // the number of states

namespace ZobristTable {
    /* initialization of Zobrist Hashing */
    std::array<std::array<uint64_t, S>, SIZE>   _init_zobrist_table(void) {
        std::array<std::array<uint64_t, S>, SIZE> table;
        std::srand(std::time(nullptr));
        for (uint16_t n = 0; n < SIZE; n++) {
            for (uint16_t s = 0; s < S; s++)
                table[n][s] = std::rand();
        }
        return (table);
    }
    const std::array<std::array<uint64_t, S>, SIZE> _table = _init_zobrist_table();
    /* the key the hashing function will use */
    struct Key {
        BitBoard p1;
        BitBoard p2;

        bool operator==(const Key &other) const {
            return (p1 == other.p1 && p2 == other.p2);
        }
    };
    /* the hashing function that std::unordered_map will use */
    struct KeyHash {
        uint64_t operator()(const Key &k) const {
            uint64_t    hash = 0;
            for (uint16_t n = 0; n < SIZE; n++)
                hash ^= _table[n][(k.p1[n] == 0 ? (k.p2[n] == 0 ? 0 : 2) : 1)]; // could use a 3rd dimension to avoid branching (opti?)
                // hash ^= _table[n][(k.p1[n]<<(n&0x3F))^(k.p2[n]<<(n&0x3F-1))>>0x3F]; // maybe faster ?
                // hash ^= _table[n][k.p1[n]][k.p2[n]]; // could use a 3rd dimension to avoid branching (opti?)
            return (hash);
        }
    };
    std::unordered_map<Key, uint16_t, KeyHash>  map;
}

#endif
