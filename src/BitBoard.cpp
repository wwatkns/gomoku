#include "BitBoard.hpp"

BitBoard::BitBoard(void) {
    this->zeros();
}

BitBoard::BitBoard(BitBoard const &src) {
    *this = src;
}

BitBoard::~BitBoard(void) {
}

BitBoard	&BitBoard::operator=(BitBoard const &src) {
    return (*this);
}

void    BitBoard::zeros(void) {
    for (uint8_t i = 0; i < N; i++)
        (*this->_values+i) = 0;
}
