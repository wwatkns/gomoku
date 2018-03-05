#include "ZobristTable.hpp"

ZobristTable::ZobristTable(void) {
    // std::srand(std::time(nullptr));
    // for (uint16_t n = 0; n < SIZE; n++) {
    //     for (uint16_t s = 0; s < S; s++) {
    //         this->_table[n][s] = std::rand();
    //         std::cout << this->_table[n][s] << std::endl;
    //     }
    // }
}

ZobristTable::ZobristTable(ZobristTable const &src) {
    *this = src;
}

ZobristTable::~ZobristTable(void) {
}

ZobristTable	&ZobristTable::operator=(ZobristTable const &rhs) {
    return (*this);
}

// ZobristTable::hash(BitBoard)
