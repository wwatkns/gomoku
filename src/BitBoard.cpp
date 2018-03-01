#include "BitBoard.hpp"

/* assignation of static variables */
std::array<int16_t, D>  BitBoard::shifts = { -19, -18, 1, 20, 19, 18, -1, -20 };
BitBoard                BitBoard::full = (std::array<uint64_t, N>){ 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFF800000 };
BitBoard                BitBoard::empty = (std::array<uint64_t, N>){ 0, 0, 0, 0, 0, 0 };
BitBoard                BitBoard::border_right = (std::array<uint64_t, N>){ 0x200004000080, 0x10000200004000, 0x800010000200004, 0x800010000200, 0x40000800010000, 0x2000040000800000 };
BitBoard                BitBoard::border_left = (std::array<uint64_t, N>){ 0x8000100002000040, 0x8000100002000, 0x400008000100002, 0x400008000100, 0x20000400008000, 0x1000020000000000 };
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

void    BitBoard::write(uint8_t x, uint8_t y) {
    uint16_t    n = (19 * y + x);
    this->values[n / BITS] |= (0x8000000000000000 >> (n % BITS));
}

void    BitBoard::remove(uint8_t x, uint8_t y) {
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


/* we light up the moves ending up in a split-three pattern

  open-split-three-left       open-split-three-right
        -*0-0-                       -*-00-
        -0*-0-                       -0-*0-
        -00-*-                       -0-0*-


close-left-split-three-left  close-left-split-three-right
         -*0-0-                        -*-00-
         -0*-0-                        -0-*0-
         -00-*-                        -0-0*-

    (...).shifted(i) & p1    :  check if the next cell contains a stone of current player
    (...).shifted(i) & empty :  check if the next cell is empty of both player
    (...).shifted_inv(i, N)  :  once we reach the last check we shift in the inverse direction N times (depending on the pattern length)
*/

/*  will compute a bitboard for all the open moves around a specific pattern,
    patterns must be encoded in big-endian, and length of pattern must be provided
    ex : -0-00- pattern will be 01011000 with length 6
          -0-0- pattern will be 01010000 with length 5
*/
// BitBoard    pattern_detector(BitBoard const &p1, BitBoard const &p2, uint8_t const &pattern, uint8_t const &length) {
//     // TODO : implement other function to test for close patterns (here this works only for open patterns)
//     BitBoard res;
//     BitBoard tmp;
//     BitBoard tmp_alt; // I don't like that it needs this extra bitboard, we could come up with a more efficient way...
//     BitBoard empty = ~p1 & ~p2;
//
//     for (uint8_t d = direction::north; d < 8; ++d) {
//         tmp = BitBoard::full;
//         for (uint8_t n = 0; n < length; n++) {
//             tmp = (d > 0 && d < 4 ? tmp & ~BitBoard::border_right : (d > 4 && d < 8 ? tmp & ~BitBoard::border_left : tmp));
//             tmp = tmp.shifted(d) & ((pattern << n & 0x80) == 0x80 ? p1 : empty);
//         }
//         tmp_alt = tmp;
//         for (uint8_t n = length-1; n > 0; n--)
//             tmp_alt |= tmp_alt.shifted_inv(d) & ((pattern << n & 0x80) == 0x80 ? p1 : empty);
//         res |= tmp_alt ^ tmp;
//     }
//     return (res & empty);
// }


/*
    we only have to check two open-three to be formed by one stone, three-four or four-four are okay.
*/
// BitBoard    forbidden_detector(BitBoard const &p1, BitBoard const &p2) {
//     BitBoard res;
//     BitBoard tmp[5];
//     BitBoard empty = ~p1 & ~p2;
//
//     uint8_t patterns[5] = { 0x50, 0x60, 0x48, 0x50 };
//     uint8_t  lengths[5] = {    5,    5,    6,    6 };
//
//     for (uint8_t p = 0; p < 5; p++) {
//         res = BitBoard::empty;
//         tmp[p] = pattern_detector(p1, p2, patterns[p], lengths[p]);
//         // std::cout << tmp[p] << std::endl;
//         for (int8_t i = p-1; i >= 0; i--)
//             res |= (tmp[p] & tmp[i]);
//     }
//     return (res);
// }

/* ================================== */

static BitBoard    pattern_detector_simple(BitBoard const &p1, BitBoard const &p2, uint8_t const &pattern, uint8_t const &length, uint8_t const &s) {
    // TODO : implement other function to test for close patterns (here this works only for open patterns)
    BitBoard res;
    BitBoard tmp;
    BitBoard empty = ~p1 & ~p2;

    for (uint8_t d = direction::north; d < 8; ++d) {
        tmp = BitBoard::full;
        for (uint8_t n = 0; n < length; n++) {
            tmp = (d > 0 && d < 4 ? tmp & ~BitBoard::border_right : (d > 4 && d < 8 ? tmp & ~BitBoard::border_left : tmp));
            tmp = tmp.shifted(d) & ((pattern << n & 0x80) == 0x80 ? p1 : empty);
        }
        res |= tmp.shifted_inv(d, s);
    }
    return (res & empty);
}

/*  will find all the positions for a given pattern, ex : -OO-O-
    -OO-O- encoded as big-endian is 01101000 or 0x68 en hexadecimal, the pattern length is 6 so you call
      $  pattern_detector(p1, p2, 0x68, 6);

    * it will return a bitboard will the bits lit where playing a stone will lead to the pattern (and in
      both way, -OO-O- pattern is also -O-OO- which is convenient as their score for evalutation are the same)

    * from -OO-O- we will generate all sub-patterns --O-O-, -O--O- and -OO--- and return the bitboards of
      their existence (or not) on the board.
*/
BitBoard    pattern_detector(BitBoard const &p1, BitBoard const &p2, uint8_t const &pattern, uint8_t const &length) {
    BitBoard    res;
    BitBoard    empty = ~p1 & ~p2;
    uint8_t     sub;

    for (uint8_t s = 0; s < length; s++) {
        sub = pattern & ~(0x80 >> s);
        if (sub != pattern)
            res |= pattern_detector_simple(p1, p2, sub, length, length-s-1);
    }
    return (res & empty);
}


/*
    -OO-O-
    -O--O-
    -----O
    --O---

    -OO-O-  |  -O-OO-  |  -OOO-  |  -OOOO-
    --------+----------+---------+---------
    -*O-O-  |  -*-OO-  |  -*OO-  |  -*OOO-
    -O*-O-  |  -O-*O-  |  -O*O-  |  -O*OO-
    -OO-*-  |  -O-O*-  |  -OO*-  |  -OO*O-
                                 |  -OOO*-

generate sub-patterns :

    uint8_t pattern = 0x58; // -O-OO-
    uint8_t length = 6;
    uint8_t sub_pattern;

    for (uint8_t i = 0; i < length; i++) {
        sub_pattern = pattern & ~(0x80 >> i);
        if (sub_pattern != pattern) {
            ... do stuff ...
        }
    }

    start sub_pattern detection, if is_empty() is false,

    or we could do pattern matching by showing the pattern we want to optain, it'll then detect if sub patterns composed from initial
    (there are as much subpatterns as lit bits in search pattern, so here 3), then we lit all the cells which contain the pattern
    and no
    -*-OO-   >   ---OO-   >   OOO--O
    -O-*O-   >   -O--O-   >   O-OO-O
    -O-O*-   >   -O-O--   >   O-O-OO



    I don't detect those :
    -OO-*-
    -*-OO-

    +---Patterns-----+---------------------+----------+-------+------------+--------+---------------------------------+
    | num |  pattern |        moves        |  binary  |  hex  | directions | length |              names              |
    +-----+----------+---------------------+----------+-------+------------+--------+---------------------------------+
    |  1  |   -O-O-  |        -O*O-        | 01010000 |  0x50 |      4     |    5   |  open three                     |
    |  2  |   -OO--  |   -OO*-  &  -*OO-   | 01100000 |  0x60 |      8     |    5   |  open three                     |
    |  3  |  -O--O-  |        -O**O-       | 01001000 |  0x48 |      4     |    6   |  open split threes              |
    |  4  |  -O-O--  |  -O*O*-  &  -*O*O-  | 01010000 |  0x50 |      8     |    6   |  open three | open split three  |
    |  5  |  -OO---  |  -OO**-  &  -**OO-  | 01100000 |  0x60 |      8     |    6   |  open three | open split three  | TODO : detect -OO-*- instead of -OO**-
    |  6  |  -O-OO-  |  -O*OO-  &  -OO*O-  | 01011000 |  0x58 |      8     |    6   |  open four                      |
    |  7  |  -OOO--  |  -OOO*-  &  -*OOO-  | 01110000 |  0x70 |      8     |    6   |  open four                      |
    +-----+----------+---------------------+----------+-------+------------+--------+---------------------------------+


    for the close pattern we could either encode a second pattern for p2 or encode as follow :
    000-  :  |000-
    -000  :  -000|
    0-0-  :  |0-0-| (if length is 5, otherwise it's |0-0-)


    two patterns are discernible, mirror ones (-00-, -000-, -0-0-, -0000-, -0--0-)
    and non-mirror (-00-0-, -0-00-, -0--, --0-, -00--, --00-, ...)
           a                    ~a              >> 1              & a              >> 1             &~a              >> 1             & a
    +-------------+  <|   +-------------+  +-------------+  +-------------+  +-------------+  +-------------+  +-------------+  +-------------+
    | . . . . . . |   |>  | 1 1 1 1 1 1 |  | . 1 1 1 1 1 |  | . . . . . . |  | . . . . . . |  | . . . . . . |  | . . . . . . |  | . . . . . . |
    | . . . . . . |  <|   | 1 1 1 1 1 1 |  | . 1 1 1 1 1 |  | . . . . . . |  | . . . . . . |  | . . . . . . |  | . . . . . . |  | . . . . . . |
    | . * 1 . 1 . |   |>  | 1 1 . 1 . 1 |  | . 1 1 . 1 . |  | . . 1 . . . |  | . . . 1 . . |  | . . . 1 . . |  | . . . . 1 . |  | . . . . 1 . |
    | . . . . . . |  <|   | 1 1 1 1 1 1 |  | . 1 1 1 1 1 |  | . . . . . . |  | . . . . . . |  | . . . . . . |  | . . . . . . |  | . . . . . . |
    | . . . . . . |   |>  | 1 1 1 1 1 1 |  | . 1 1 1 1 1 |  | . . . . . . |  | . . . . . . |  | . . . . . . |  | . . . . . . |  | . . . . . . |
    | . . . . . . |  <|   | 1 1 1 1 1 1 |  | . 1 1 1 1 1 |  | . . . . . . |  | . . . . . . |  | . . . . . . |  | . . . . . . |  | . . . . . . |
    +-------------+   |>  +-------------+  +-------------+  +-------------+  +-------------+  +-------------+  +-------------+  +-------------+
*/

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
