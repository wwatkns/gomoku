#include "BitBoard.hpp"

/* assignation of static variables */
std::array<int16_t, D>  BitBoard::shifts = { -19, -18, 1, 20, 19, 18, -1, -20 };
BitBoard                BitBoard::full = (std::array<uint64_t, N>){ 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFF800000 };
BitBoard                BitBoard::empty = (std::array<uint64_t, N>){ 0, 0, 0, 0, 0, 0 };
BitBoard                BitBoard::border_right = (std::array<uint64_t, N>){ 0x200004000080, 0x10000200004000, 0x800010000200004, 0x800010000200, 0x40000800010000, 0x2000040000800000 };
BitBoard                BitBoard::border_left = (std::array<uint64_t, N>){ 0x8000100002000040, 0x8000100002000, 0x400008000100002, 0x400008000100, 0x20000400008000, 0x1000020000000000 };
BitBoard                BitBoard::border_top = (std::array<uint64_t, N>){ 0xFFFFE00000000000, 0, 0, 0, 0, 0 };
BitBoard                BitBoard::border_bottom = (std::array<uint64_t, N>){ 0, 0, 0, 0, 0, 0x3FFFF800000 };
std::array<t_pattern,11> BitBoard::patterns = {
    (t_pattern){ 0x70, 5, 4,  30 },  //   -OOO-  :  open three
    (t_pattern){ 0x68, 6, 8,  40 },  //  -OO-O-  :  open split three
    (t_pattern){ 0x78, 6, 4, 220 },  //  -OOOO-  :  open four
    (t_pattern){ 0xE0, 4, 8,   5 },  //    OOO-  :  close three
    (t_pattern){ 0xD0, 5, 8,  10 },  //   OO-O-  :  close split three #1
    (t_pattern){ 0xB0, 5, 8,  15 },  //   O-OO-  :  close split three #2
    (t_pattern){ 0xF0, 5, 8,  25 },  //   OOOO-  :  close four
    (t_pattern){ 0x5C, 6, 8,   0 },  //  -O-OOO  :  split four #1
    (t_pattern){ 0x6C, 6, 8,   0 },  //  -OO-OO  :  split four #2
    (t_pattern){ 0x74, 6, 8,   0 },  //  -OOO-O  :  split four #3
    (t_pattern){ 0xF8, 5, 4, 255 }   //   OOOOO  :  five
};


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

void    BitBoard::write(uint8_t x, uint8_t y) {
    uint16_t    n = (19 * y + x);
    this->values[n / BITS] |= (0x8000000000000000 >> (n % BITS));
}

void    BitBoard::remove(uint8_t x, uint8_t y) {
    uint16_t    n = (19 * y + x);
    this->values[n / BITS] &= ~(0x8000000000000000 >> (n % BITS));
}

uint16_t    BitBoard::set_count(void) const {
    uint64_t    tmp;
    uint16_t    res = 0;

    for (uint8_t i = 0; i < N; i++) {
        tmp = this->values[i];
        for (; tmp; res++)
            tmp &= tmp - 1;
    }
    return (res);
}

bool    BitBoard::check_bit(uint8_t x, uint8_t y) const {
    uint16_t    n = (19 * y + x);
    return ((this->values[n / BITS] & (0x8000000000000000 >> (n % BITS))) == 0 ? false : true);
}

bool    BitBoard::is_empty(void) const {
    for (uint8_t i = 0; i < N; i++)
        if (this->values[i] != 0)
            return (false);
    return (true);
}

BitBoard    BitBoard::opens(void) const {
    return (~(*this));
}

BitBoard    BitBoard::neighbours(void) const {
    return (this->dilated() ^ *this);
}

/* shift either right or left depending on the sign of the shift, if we shift right we handle the overflow to the extra bits */
BitBoard    BitBoard::shifted(uint8_t dir, uint8_t n) const {
    return (BitBoard::shifts[dir] > 0 ? (*this >> BitBoard::shifts[dir]*n) & BitBoard::full : (*this << -BitBoard::shifts[dir]*n));
}

/* shift in the inverse direction from BitBoard::shifted (useful for pattern detection) */
BitBoard    BitBoard::shifted_inv(uint8_t dir, uint8_t n) const {
    return (BitBoard::shifted(dir < 4 ? dir + 4 : dir - 4, n));
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

BitBoard    BitBoard::rotated_45(void) {
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

/* return the neighboring cells of both players */
BitBoard    get_all_neighbours(BitBoard const &p1, BitBoard const &p2) {
    return ((p1.dilated() | p2.dilated()) ^ (p1 | p2));
}

/* return the open cells of both players */
BitBoard    get_all_open_cells(BitBoard const &p1, BitBoard const &p2) {
    return (~(p1 | p2));
}

/* return the occupied cells of both players */
BitBoard    get_all_occupied_cells(BitBoard const &p1, BitBoard const &p2) {
    return (p1 | p2);
}

/* will return the interesting positions to explore for p1,
   it creates a star pattern of size 2 around p1 positions taking into account
   p2 pieces (so a potential position behind an enemy stone will be ignored)
   +-----------+
   | 1 . 1 . 1 |
   | . 1 1 1 . |
   | 1 1 . 1 1 |
   | . 1 1 1 . |
   | 1 . 1 . 1 |
   +-----------+
*/
BitBoard    get_player_open_adjacent_positions(BitBoard const &p1, BitBoard const &p2) {
    BitBoard res = p1.dilated();
    for (uint8_t i = direction::north_east; i < direction::south; ++i)
        res |= (p1.shifted(i) & p2).shifted(i) ^ p1.shifted(i, 2) & ~BitBoard::border_left;
    for (uint8_t i = direction::south_west; i < 8; ++i)
        res |= (p1.shifted(i) & p2).shifted(i) ^ p1.shifted(i, 2) & ~BitBoard::border_right;
    res |= (p1.shifted(direction::north) & p2).shifted(direction::north) ^ p1.shifted(direction::north, 2);
    res |= (p1.shifted(direction::south) & p2).shifted(direction::south) ^ p1.shifted(direction::south, 2);
    return (res & ~p1 & ~p2);
}

/* will return the bitboard of all the open positions for open pair captures for p1,
   I dont't even know how I made this algorithm, but it works alright ! could be
   adapted to the patterns matching to get all the positions of future open-threes, ...
*/
BitBoard    get_player_open_pairs_captures_positions(BitBoard const &p1, BitBoard const &p2) {
    BitBoard res;
    for (uint8_t i = direction::north_east; i < direction::south; ++i)
        res |= ((p1.shifted(i) & p2).shifted(i) & p2).shifted(i) & p1.shifted(i, 3) & ~BitBoard::border_left;
    for (uint8_t i = direction::south_west; i < 8; ++i)
        res |= ((p1.shifted(i) & p2).shifted(i) & p2).shifted(i) & p1.shifted(i, 3) & ~BitBoard::border_right;
    res |= ((p1.shifted(direction::north) & p2).shifted(direction::north) & p2).shifted(direction::north) & p1.shifted(direction::north, 3);
    res |= ((p1.shifted(direction::south) & p2).shifted(direction::south) & p2).shifted(direction::south) & p1.shifted(direction::south, 3);
    return (res & ~p1 & ~p2);
}

static BitBoard sub_pattern_detector_alt(BitBoard const &p1, BitBoard const &p2, uint8_t const &pattern, uint8_t const &length, uint8_t const &s, uint8_t const &type, uint8_t const &dir) {
    BitBoard    res;
    BitBoard    open_cells = (~p1 & ~p2);

    res = (type == 0x80 ? p2 : BitBoard::full);
    for (uint8_t n = 0; n < length; n++) {
        res = (dir > 0 && dir < 4 ? res & ~BitBoard::border_right : (dir > 4 && dir < 8 ? res & ~BitBoard::border_left : res));
        res = res.shifted(dir) & ((pattern << n & 0x80) == 0x80 ? p1 : open_cells);
    }
    return (res.shifted_inv(dir, s) & open_cells);
}

/*  will return a bitboard showing the forbidden cells for p1, it achieves that
    by searching for open threes patterns and checking if we have overlapping open
    moves leading to those. It's not the most efficient algorithm, we must improve
    it in the future, but it is robust.
*/
BitBoard        forbidden_detector(BitBoard const &p1, BitBoard const &p2) {
    BitBoard    res;
    BitBoard    tmp[36]; // 36 is patterns * sub_patterns * directions
    BitBoard    open_cells = (~p1 & ~p2);
    uint8_t     j;
    uint8_t     sub;
    uint8_t     type;
    uint8_t     patterns[3] = { 0x58, 0x68, 0x70 }; // -O-OO- , -OO-O- , -OOO-
    uint8_t      lengths[3] = {    6,    6,    5 };

    for (uint8_t p = 0; p < 3; p++) { // iterate through patterns
        type = (patterns[p] & 0x80) | (0x1 << (8-lengths[p]) & patterns[p]);
        j = 0;
        for (uint8_t s = 0; s < lengths[p]; s++) { // iterate through sub patterns
            sub = patterns[p] & ~(0x80 >> s);
            if (sub != patterns[p]) {
                for (uint8_t d = direction::north; d < 4; ++d) { // iterate through directions
                    tmp[(p*12+j*4)+d] = sub_pattern_detector_alt(p1, p2, sub, lengths[p], lengths[p]-s-1, type, d);
                    for (int8_t i = (p*12+j*4)+d-1; i >= 0; i--) // TODO : optimize this sub_pattern overlap loop, for now we loop 615 times total here
                        res |= (tmp[(p*12+j*4)+d] & tmp[i]);
                }
                j++;
            }
        }
    }
    return (res & open_cells);
}

/*  TODO :
    -> implement first iteration of evaluation function with the pattern_detector
*/

static BitBoard sub_pattern_detector(BitBoard const &p1, BitBoard const &p2, t_pattern const &pattern, uint8_t const &s, uint8_t const &type) {
    BitBoard    res;
    BitBoard    tmp;
    BitBoard    open_cells = (~p1 & ~p2);

    for (uint8_t d = direction::north; d < pattern.dirs; ++d) {
        tmp = (type == 0x80 ? p2 : BitBoard::full);
        for (uint8_t n = 0; n < pattern.size; n++) {
            tmp = (d > 0 && d < 4 ? tmp & ~BitBoard::border_right : (d > 4 && d < 8 ? tmp & ~BitBoard::border_left : tmp));
            tmp = tmp.shifted(d) & ((pattern.repr << n & 0x80) == 0x80 ? p1 : open_cells);
        }
        res |= tmp.shifted_inv(d, s);
    }
    return (res & open_cells);
}

/*  will return the bitboard showing the open moves for p1 leading to the specified pattern.
    the patterns must be encoded in big-endian and have a length defined (-OO-O- is 01101000
    so 0x68 with length 6) see the Patterns table in BitBoard.hpp for all the patterns.
*/
BitBoard        pattern_detector(BitBoard const &p1, BitBoard const &p2, t_pattern const &pattern) {
    BitBoard    res;
    t_pattern   sub = pattern;
    uint8_t     type = (pattern.repr & 0x80) | (0x1 << (8-pattern.size) & pattern.repr);

    for (uint8_t s = 0; s < pattern.size; s++) {
        sub.repr = pattern.repr & ~(0x80 >> s);
        if (sub.repr != pattern.repr)
            res |= sub_pattern_detector(p1, p2, sub, pattern.size-s-1, type); // TODO : move code from sub_pattern_detector here (optimization)
    }
    return (res & ~p1 & ~p2);
}

/*  Cannot handle the same pattern yet (it should check different directions similarly as forbidden_detector)
*/
BitBoard    double_pattern_detector(BitBoard const &p1, BitBoard const &p2, t_pattern const &pattern1, t_pattern const &pattern2) {
    return (pattern_detector(p1, p2, pattern1) & pattern_detector(p1, p2, pattern2));
}

bool        detect_five_aligned(BitBoard const &bitboard) {
    BitBoard    tmp;
    for (uint8_t i = 0; i < D; i++) {
        tmp = bitboard;
        for(uint8_t n = 1; (tmp &= tmp.shifted(i)).is_empty() == false; ++n)
            if (n >= 4)
                return (true);
    }
    return (false);
}

// detect a pair and return the positions on the bitboard where it leads to capture
BitBoard    pair_capture_detector(BitBoard const &p1, BitBoard const &p2) {
    BitBoard    res;
    BitBoard    tmp;
    BitBoard    open_cells = (~p1 & ~p2);

    for (uint8_t d = direction::north; d < 8; ++d) {
        tmp = p1;
        for (uint8_t n = 0; n < 3; n++) {
            tmp = (d > 0 && d < 4 ? tmp & ~BitBoard::border_right : (d > 4 && d < 8 ? tmp & ~BitBoard::border_left : tmp));
            tmp = tmp.shifted(d) & ((0xC0 << n & 0x80) == 0x80 ? p2 : open_cells);
        }
        res |= (tmp.shifted_inv(d, 2) | tmp.shifted_inv(d, 1) | tmp);
    }
    return (res);
}

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
