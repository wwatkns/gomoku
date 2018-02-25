#include "BitBoard.hpp"

/* assignation of static variables */
// std::array<int16_t, D>  BitBoard::shifts = { -20, -19, 1, 21, 20, 19, -1, -21 };
// BitBoard                BitBoard::full_mask = (std::array<uint64_t, N>){ 0xFFFFEFFFFEFFFFEF, 0xFFFEFFFFEFFFFEFF, 0xFFEFFFFEFFFFEFFF, 0xFEFFFFEFFFFEFFFF, 0xEFFFFEFFFFEFFFFE, 0xFFFFEFFFFEFFFFE0 };
std::array<int16_t, D>  BitBoard::shifts = { -19, -18, 1, 20, 19, 18, -1, -20 };
BitBoard                BitBoard::full_mask = (std::array<uint64_t, N>){ 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFF800000 };
BitBoard                BitBoard::empty_mask = (std::array<uint64_t, N>){ 0, 0, 0, 0, 0, 0 };

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
        res.values[i] = (~this->values[i]) & BitBoard::full_mask[i];
	return (res);
}

BitBoard    BitBoard::operator>>(uint16_t shift) const {
    BitBoard	res;
    if (shift < BITS) {
        for (uint8_t i = N-1; i > 0; i--)
            res.values[i] = (this->values[i] >> shift) | (this->values[i-1] << (BITS - shift));
        res.values[0] = (this->values[0] >> shift);
    } else {
        uint16_t    n = shift / BITS;
        uint16_t    a = shift % BITS;
        for (uint8_t i = N-1; i > n; i--) {
            if (a == 0)
                res.values[i] = (this->values[i-n] << (BITS - a));
            else
                res.values[i] = (this->values[i-n] >> a) | (this->values[i-(n+1)] << (BITS - a));
        }
        res.values[n] = (this->values[n] >> a);
    }
	return (res);
}

BitBoard    BitBoard::operator<<(uint16_t shift) const {
    BitBoard	res;
    if (shift < BITS) {
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

/*
** Member access operator overload
*/
uint64_t    &BitBoard::operator[](uint8_t i){
    return (this->values[i]);
}




/*
** Non-member functions
*/
BitBoard    dilation(BitBoard const &bitboard, uint8_t amount) { // TODO : remove amount if we don't use it
	BitBoard	res = bitboard;

    for (uint8_t i = 0; i < D; i++) // TODO : something more intelligent than those conditions
        if (BitBoard::shifts[i] > 0)
            res |= (bitboard >>  BitBoard::shifts[i]);
        else
            res |= (bitboard << -BitBoard::shifts[i]);
    return (res);
}

// bool        match_pattern() {
//
// }

// std::ostream	&operator<<(std::ostream &os, BitBoard const &bitboard) {
//     std::string         tmp = "";
//     std::string         sub = "";
//     std::stringstream   ss;
//
//     for (uint8_t i = 0; i < N; i++)
//         tmp += std::bitset<64>(bitboard.values[i]).to_string();
//
//     for (uint32_t i = 0; i < 19; i++) {
//         sub = tmp.substr(i*20, 20);
//         for (uint32_t j = 0; j < 20; j++)
//             ss << (((i*20)+j)%BITS!=0?" ":"/") << (sub[j]=='0'?"◦":"◉");
//         ss << std::endl;
//     }
//     os << ss.str();
// 	return (os);
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
