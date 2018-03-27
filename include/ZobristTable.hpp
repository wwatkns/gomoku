#ifndef ZOBRISTTABLE_HPP
# define ZOBRISTTABLE_HPP

# include <iostream>
# include <cstdlib>
# include <random>
# include <array>
# include <unordered_map>
# include "BitBoard.hpp"

# define SIZE 361   // the number of cells on the board (19*19)
# define S 3        // the number of states

typedef struct  s_stored {
    int         score;
    int         move;
    int8_t      depth;          /* the actual depth in the search tree */
    int8_t      max_id_depth;   /* the iterative deepening max depth at which the node was evaluated */
    uint8_t     flag;
}               t_stored;

namespace ZobristTable {
    enum flag {
        exact,
        lowerbound,
        upperbound
    };

    /* initialization of Zobrist Hashing */
    static const std::array<std::array<uint64_t, S>, SIZE>   _init_zobrist_table_x64(void) {
        std::array<std::array<uint64_t, S>, SIZE> table;
        /* set up random generator 64-bit */
        std::random_device  rd;
        std::mt19937_64     e2(rd());
        std::uniform_int_distribution<unsigned long long>   dist(std::llround(std::pow(2, 61)), std::llround(std::pow(2, 62)));

        for (int n = 0; n < SIZE; n++)
            for (int s = 0; s < S; s++)
                table[n][s] = dist(e2);
        return (table);
    }
    static const std::array<std::array<uint64_t, S>, SIZE> _table = _init_zobrist_table_x64();

    /* the key the hashing function will use */
    struct Key {
        BitBoard    p1;
        BitBoard    p2;

        bool operator==(const Key &other) const {
            return (p1 == other.p1 && p2 == other.p2);
        }
    };
    /* the hashing function that std::unordered_map will use */
    struct KeyHash {
        uint64_t operator()(const Key &k) const {
            uint64_t    hash = 0;
            for (int n = 0; n < SIZE; ++n)
                hash ^= _table[n][(!k.p1[n] ? (!k.p2[n] ? 0 : 2) : 1)];
                // hash ^= _table[n][(k.p1[n]<<(n&0x3F))^(k.p2[n]<<(n&0x3F-1))>>0x3F]; // maybe faster than branching ?
            return (hash);
        }
    };
}

#endif
