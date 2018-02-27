#include "BitBoard.hpp"

/* assignation of static variables */
std::array<int16_t, D>  BitBoard::shifts = { -19, -18, 1, 20, 19, 18, -1, -20 };
BitBoard                BitBoard::full = (std::array<uint64_t, N>){ 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFF800000 };
BitBoard                BitBoard::empty = (std::array<uint64_t, N>){ 0, 0, 0, 0, 0, 0 };
BitBoard                BitBoard::border_right = (std::array<uint64_t, N>){ 0x200004000080, 0x10000200004000, 0x800010000200004, 0x800010000200, 0x40000800010000, 0x2000040000800010 };
BitBoard                BitBoard::border_left = (std::array<uint64_t, N>){ 0x8000100002000040, 0x8000100002000, 0x400008000100002, 0x400008000100, 0x20000400008000, 0x1000020000400008 };
BitBoard                BitBoard::border_top = (std::array<uint64_t, N>){ 0xFFFFE00000000000, 0, 0, 0, 0, 0 };
BitBoard                BitBoard::border_bottom = (std::array<uint64_t, N>){ 0, 0, 0, 0, 0, 0x3FFFF800000 };

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

BitBoard	&BitBoard::operator=(uint64_t const &val) {
    this->values[4] = (val >> 41);
    this->values[5] = (val << 23);
    return (*this);
}

/*
** Helper functions
*/
void    BitBoard::zeros(void) {
    for (uint8_t i = 0; i < N; i++)
        this->values[i] = 0;
}

/* will broadcast the given row (only the first 19 bits) to all rows,
   the pattern must be encoded in the first 19 bits */
void    BitBoard::broadcast_row(uint64_t row) {
    uint16_t    offset = 19;
    int32_t     shift = 0;

    row >>= BITS-19;
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

void    BitBoard::write_move(uint8_t x, uint8_t y) {
    uint16_t    n = (19 * y + x);
    this->values[n / BITS] |= (0x8000000000000000 >> (n % BITS));
}

void    BitBoard::delete_move(uint8_t x, uint8_t y) {
    uint16_t    n = (19 * y + x);
    this->values[n / BITS] &= ~(0x8000000000000000 >> (n % BITS));
}

bool    BitBoard::is_empty(void) {
    for (uint8_t i = 0; i < N; i++)
        if (this->values[i] != 0)
            return (true);
    return (false);
}

BitBoard    BitBoard::opens(void) const {
    return (~(*this));
}

BitBoard    BitBoard::neighbours(void) const {
    return (dilation(*this) ^ *this);
}

/* shift either right or left depending on the sign of the shift, if we shift right we handle the overflow to the extra bits */
BitBoard    BitBoard::shifted(uint8_t dir, uint8_t n) const {
    return (BitBoard::shifts[dir] > 0 ? (*this >> BitBoard::shifts[dir]*n) & BitBoard::full : (*this << -BitBoard::shifts[dir]*n));
}

/* return the dilated board taking into account the board boundaries */
BitBoard    BitBoard::dilated(void) const {
	BitBoard	res = *this;
    for (uint8_t i = direction::north_east; i < direction::south; ++i)
        res |= this->shifted(i) & ~BitBoard::border_left;
    for (uint8_t i = direction::south_west; i < 8; ++i)
        res |= this->shifted(i) & ~BitBoard::border_right;
    res |= this->shifted(direction::north);
    res |= this->shifted(direction::south);
    return (res);
}

/* return the eroded board taking into account the board boundaries */
BitBoard    BitBoard::eroded(void) const {
	BitBoard	res = *this;
    for (uint8_t i = direction::north_east; i < direction::south; ++i)
        res &= this->shifted(i) & ~BitBoard::border_left;
    for (uint8_t i = direction::south_west; i < 8; ++i)
        res &= this->shifted(i) & ~BitBoard::border_right;
    res &= this->shifted(direction::north);
    res &= this->shifted(direction::south);
    return (res);
}

// BitBoard    BitBoard::rotated_315(void) {
//     /* non optimized */
// }

BitBoard    BitBoard::rotate_45(void) {
    /* non optimized, but working */
    BitBoard    res;
    BitBoard    mask;
    uint64_t    v = 1;

    v <<= BITS - 1;
    for (uint64_t j = 0; j < 19; j++) {
        mask.broadcast_row(v);
        res |= ((*this >> (19 * j) | *this << (19 * (19 - j)))) & mask;
        v >>= 1;
    }
    return (res);
}

/*
** Arithmetic operator overload
*/
BitBoard	BitBoard::operator|(const BitBoard &rhs) const {
	BitBoard	res;
    for (uint8_t i = 0; i < N; i++)
        res.values[i] = this->values[i] | rhs.values[i];
	return (res);
}

BitBoard	BitBoard::operator&(const BitBoard &rhs) const {
	BitBoard	res;
    for (uint8_t i = 0; i < N; i++)
        res.values[i] = this->values[i] & rhs.values[i];
	return (res);
}

BitBoard    BitBoard::operator^(BitBoard const &rhs) const {
    BitBoard	res;
    for (uint8_t i = 0; i < N; i++)
        res.values[i] = this->values[i] ^ rhs.values[i];
    return (res);
}

BitBoard	BitBoard::operator~(void) const {
	BitBoard	res;
    for (uint8_t i = 0; i < N; i++)
        res.values[i] = ~this->values[i];
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
** Comparison operator overload
*/
bool        BitBoard::operator==(BitBoard const &rhs) {
    for (uint8_t i = 0; i < N; i++)
        if (this->values[i] != rhs.values[i])
            return (false);
    return (true);
}

bool        BitBoard::operator!=(BitBoard const &rhs) {
    for (uint8_t i = 0; i < N; i++)
        if (this->values[i] != rhs.values[i])
            return (true);
    return (false);
}

/*
** Non-member functions
*/
BitBoard    dilation(BitBoard const &bitboard) { // does not take into account the board 2d topology
	BitBoard	res = bitboard;
    for (uint8_t i = 0; i < D; i++)
        res |= bitboard.shifted(i);
    return (res);
}

BitBoard    erosion(BitBoard const &bitboard) { // does not take into account the board 2d topology
	BitBoard	res = bitboard;
    for (uint8_t i = 0; i < D; i++)
        res &= bitboard.shifted(i);
    return (res);
}


/* return the neighboring cells of both players */
BitBoard    get_all_neighbours(BitBoard const &p1, BitBoard const &p2) {
    return (dilation(p1 | p2) ^ (p1 | p2));
}

/* return the open cells of both players */
BitBoard    get_all_open_cells(BitBoard const &p1, BitBoard const &p2) {
    return (~(p1 | p2));
}

/* return the occupied cells of both players */
BitBoard    get_all_occupied_cells(BitBoard const &p1, BitBoard const &p2) {
    return (p1 | p2);
}

bool        detect_five_aligned(BitBoard const &bitboard) {
    BitBoard    tmp;
    for (uint8_t i = 0; i < D; i++) {
        tmp = bitboard;
        for(uint8_t n = 1; (tmp &= tmp.shifted(i)).is_empty(); ++n)
            if (n >= 4)
                return (true);
    }
    return (false);
}

// bool        detect_test(BitBoard const &bitboard) {
//     BitBoard    tmp;
//     uint8_t     pattern = 0x1A << 1; // it's a pattern of size 6 but we bitshift 8 - (2 + 1) = 1
//     // & 0x80
//     // for (uint8_t i = 0; i < D; i++) {
//     for (uint8_t i = 2; i < 3; i++) {
//         tmp = bitboard;
//         // for(uint8_t n = 1; (tmp = ((pattern << n & 0x80)==0 ?tmp & tmp.shifted(i):tmp ^ tmp.shifted(i)) ).is_empty(); ++n) {
//         for(uint8_t n = 1; n < 6; ++n) {
//             tmp = tmp & (((pattern<<(n-1)&0x80)>>7) ? ~tmp : tmp);
//             tmp &= tmp.shifted(i);
//             // std::cout << ((pattern<<(n-1)&0x80)>>7) << std::endl;
//             // std::cout << (((pattern<<(n-1)&0x80)>>7) ? tmp : ~tmp) << std::endl;
//             std::cout << tmp << std::endl;
//             // if (n >= 4)
//                 // return (true);
//             // if (tmp.is_empty() == false)
//                 // break;
//         }
//     }
//     return (false);
// }

// bool        detect_gomoku_pattern_around(BitBoard const &p1, BitBoard const &p2, uint8_t const &pattern, uint8_t const &x, uint8_t const &y) {
// }

// bool       find_pattern(BitBoard const &bitboard, uint64_t pattern) {
// }

// bool       rfind_pattern(BitBoard const &bitboard, uint64_t pattern) {
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
    /* show the extra bits */
    // sub = tmp.substr(19*19, 23);
    // for (uint32_t j = 0; j < 21; j++)
    //     ss << (((19*19)+j)%BITS!=0?" ":"/") << (sub[j]=='0'?"◦":"◉");
    // os << ss.str()  << std::endl;
    os << ss.str();
	return (os);
}
