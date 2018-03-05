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

std::array<std::array<uint64_t, S>, SIZE>   init_zobrist_table(void) {
    std::array<std::array<uint64_t, S>, SIZE> table;
    std::srand(std::time(nullptr));
    for (uint16_t n = 0; n < SIZE; n++) {
        for (uint16_t s = 0; s < S; s++) {
            table[n][s] = std::rand();
            std::cout << table[n][s] << std::endl;
        }
    }
    return (table);
}

// _table[cell][state]
const std::array<std::array<uint64_t, S>, SIZE>       _table = init_zobrist_table();

struct Key {
    BitBoard p1;
    BitBoard p2;
};

struct KeyHash {
    uint64_t operator()(const Key &k) const {
        uint64_t    hash = 0;
        for (uint16_t n = 0; n < SIZE; n++)
            hash ^= _table[n][(k.p1[n] == 0 ? (k.p2[n] == 0 ? 0 : 2) : 1)];
        return (hash);
    }
};

class ZobristTable {

public:
    ZobristTable(void);
    ZobristTable(ZobristTable const &src);
    ~ZobristTable(void);
    ZobristTable	&operator=(ZobristTable const &rhs);

private:
    std::unordered_map<Key, uint16_t, KeyHash>  _hash_map;
    std::array<std::array<uint64_t, S>, SIZE>   _table = init_zobrist_table();
};


#endif
