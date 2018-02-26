#include "BitBoard.hpp"

/* assignation of static variables */
std::array<int16_t, D>  BitBoard::shifts = { -19, -18, 1, 20, 19, 18, -1, -20 };
BitBoard                BitBoard::full = (std::array<uint64_t, N>){ 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFF800000 };
BitBoard                BitBoard::empty = (std::array<uint64_t, N>){ 0, 0, 0, 0, 0, 0 };

BitBoard::BitBoard(void) {
    this->zeros();
}

BitBoard::BitBoard(std::array<uint64_t, N> values) {
    this->values = values;
}

BitBoard::BitBoard(BitBoard const &src) {
    *this = src;
}

BitBoard::~BitBoard(void) {
}

BitBoard	&BitBoard::operator=(BitBoard const &rhs) {
    this->values = rhs.values;
    return (*this);
}

/*
** Helper functions
*/
void    BitBoard::zeros(void) {
    for (uint8_t i = 0; i < N; i++)
        this->values[i] = 0;
}

void    BitBoard::broadcastRow(uint64_t row) {
    /* will broadcast the given row (only the first 19 bits) to all rows,
        the pattern must be encoded */
    uint16_t    offset = 19;
    int32_t     shift = 0;

    row >>= BITS-19; /* if the pattern is encoded in the first 19 bits */
    this->zeros();
    for (uint8_t i = 0; i < N; i++) {
        shift = 1;
        for (uint8_t j = 0; shift > 0; j++) {
            shift = BITS - offset - (19 * j);
            this->values[i] |= (shift > 0 ? row << shift : row >> -shift);
        }
        offset = -shift;
    }
}


// static uint64_t rotateLeft(uint64_t v, uint8_t amount) { // rotate bits to the left
//     return (v << amount) | v >> (BITS - amount);
// }
//
// static uint64_t rotateRight(uint64_t v, uint8_t amount) { // rotate bits to the right
//     return (v >> amount) | v << (BITS - amount);
// }

// BitBoard    BitBoard::rotatedLeft45(void) {
// }
BitBoard    BitBoard::rotateRight45(void) {
    BitBoard    res;
    BitBoard    mask;
    uint64_t    val = 1;

    val <<= BITS - 1;
    for (uint64_t j = 0; j < 19; j++) {
        mask.broadcastRow(val);
        res |= ((*this >> (19 * j) | *this << (19 * (19 - j)))) & mask;
        std::cout << (*this >> (19 * j)) << std::endl;
        // std::cout << (((*this >> (19 * j)) | (*this << (19 * (19 - j)))) & mask) << std::endl;
        val >>= 1;
    }
    return (res);
}


/*
** Arithmetic operator overload
*/
BitBoard	BitBoard::operator|(const BitBoard &rhs) {
	BitBoard	res;
    for (uint8_t i = 0; i < N; i++)
        res.values[i] = this->values[i] | rhs.values[i];
	return (res);
}

BitBoard	BitBoard::operator&(const BitBoard &rhs) {
	BitBoard	res;
    for (uint8_t i = 0; i < N; i++)
        res.values[i] = this->values[i] & rhs.values[i];
	return (res);
}

BitBoard    BitBoard::operator^(BitBoard const &rhs) {
    BitBoard	res;
    for (uint8_t i = 0; i < N; i++)
        res.values[i] = this->values[i] ^ rhs.values[i];
    return (res);
}

BitBoard	BitBoard::operator~(void) {
	BitBoard	res;
    for (uint8_t i = 0; i < N; i++)
        res.values[i] = (~this->values[i]) & BitBoard::full[i];
	return (res);
}

BitBoard    BitBoard::operator>>(int32_t shift) const {
    BitBoard	res;
    if (shift <= 0)
        return (*this);
    else if (shift < BITS) {
        for (uint8_t i = N-1; i > 0; i--)
            res.values[i] = (this->values[i] >> shift) | (this->values[i-1] << (BITS - shift));
        res.values[0] = (this->values[0] >> shift);
    } else {
        uint16_t    n = shift / BITS;
        uint16_t    a = shift % BITS;
        std::cout << "n: " << n << std::endl;
        std::cout << "a: " << a << std::endl;
        for (uint8_t i = N-1; i > n; i--) {
            if (a == 0)
                res.values[i] = (this->values[i-n] << (BITS - a));
            else
                res.values[i] = (this->values[i-n] >> a) | (this->values[i-(n+1)] << (BITS - a));
        }
        res.values[n] = (this->values[0] >> a);
    }
	return (res);
}

BitBoard    BitBoard::operator<<(int32_t shift) const {
    BitBoard	res;
    if (shift <= 0)
        return (*this);
    else if (shift < BITS) {
        for (uint8_t i = 0; i < N; i++)
            res.values[i] = (this->values[i] << shift) | (this->values[i+1] >> (BITS - shift));
        res.values[N-1] = (this->values[N-1] << shift);
    } else {
        uint16_t    n = shift / BITS;
        uint16_t    a = shift % BITS;
        uint16_t    p = N - (n + 1);
        for (uint8_t i = 0; i < p; i++) {
            if (a == 0)
                res.values[i] = (this->values[i+n] >> (BITS - a));
            else
                res.values[i] = (this->values[i+n] << a) | (this->values[i+n+1] >> (BITS - a));
        }
        res.values[p] = (this->values[p+n] << a);
    }
	return (res);
}

/*
** Assignation operator overload
*/
BitBoard    &BitBoard::operator|=(BitBoard const &rhs) {
    for (uint8_t i = 0; i < N; i++)
        this->values[i] = (this->values[i] | rhs.values[i]);
    return (*this);
}

BitBoard    &BitBoard::operator&=(BitBoard const &rhs) {
    for (uint8_t i = 0; i < N; i++)
        this->values[i] = (this->values[i] & rhs.values[i]);
    return (*this);
}

BitBoard    &BitBoard::operator^=(BitBoard const &rhs) {
    for (uint8_t i = 0; i < N; i++)
        this->values[i] = (this->values[i] ^ rhs.values[i]);
    return (*this);
}

BitBoard    &BitBoard::operator<<=(int32_t shift) {
    *this = (*this << shift);
    return (*this);
}

BitBoard    &BitBoard::operator>>=(int32_t shift) {
    *this = (*this >> shift);
    return (*this);
}

/*
** Member access operator overload
*/
uint64_t    &BitBoard::operator[](uint8_t i){
    return (this->values[i]);
}


/*
** Non-member functions
*/
BitBoard    dilation(BitBoard const &bitboard) {
	BitBoard	res = bitboard;
    for (uint8_t i = 0; i < D; i++)
        res |= (BitBoard::shifts[i] > 0 ? (bitboard >>  BitBoard::shifts[i]) : (bitboard << -BitBoard::shifts[i]));
    return (res);
}

BitBoard    erosion(BitBoard const &bitboard) {
	BitBoard	res = bitboard;
    for (uint8_t i = 0; i < D; i++)
        res &= (BitBoard::shifts[i] > 0 ? (bitboard >>  BitBoard::shifts[i]) : (bitboard << -BitBoard::shifts[i]));
    return (res);
}

BitBoard    get_neighbours(BitBoard const &bitboard) {
	BitBoard	res;
    res = dilation(bitboard) ^ bitboard;
    return (res);
}


/*
    a pattern should be able to fit on a 64bit variable if we use rotated BitBoards
    to check for matches (as the pattern would fit on successive bits, ex :
        010110 would be a variant of open three).
*/
// bool       find_pattern(BitBoard const &bitboard, uint64_t pattern) {
// }
//
// bool       rfind_pattern(BitBoard const &bitboard, uint64_t pattern) {
// }
//
// bool       count_pattern(BitBoard const &bitboard, uint64_t pattern) {
// }

// bool        match_pattern() {
//
// }

std::ostream	&operator<<(std::ostream &os, BitBoard const &bitboard) {
    std::string         tmp = "";
    std::string         sub = "";
    std::stringstream   ss;

    for (uint8_t i = 0; i < N; i++)
        tmp += std::bitset<64>(bitboard.values[i]).to_string();

    for (uint32_t i = 0; i < 19; i++) {
        sub = tmp.substr(i*19, 19);
        for (uint32_t j = 0; j < 19; j++)
            ss << (((i*19)+j)%BITS!=0?" ":"/") << (sub[j]=='0'?"◦":"◉");
        ss << std::endl;
    }
    os << ss.str();
	return (os);
}

/*

*/
