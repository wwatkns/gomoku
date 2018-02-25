#ifndef BITBOARD_HPP
# define BITBOARD_HPP

# include <iostream>
# include <cstdlib>
# include <array>
# include <bitset> // for display
# include <string> // for display
# include <sstream>

# define N 6
# define D 8
# define BITS 64

/*  Implementation of a bitboard representation of a square board of 19*19 using
    6 long integers (64bits), and bit operations.
*/
class BitBoard {

public:
    BitBoard(void);
    BitBoard(std::array<uint64_t, N> values);
    BitBoard(BitBoard const &src);
    ~BitBoard(void);
    BitBoard	&operator=(BitBoard const &rhs);

    void        zeros(void);

    /* arithmetic (bitwise) operator overload */
    BitBoard    operator|(BitBoard const &rhs);     // bitwise union
    BitBoard    operator&(BitBoard const &rhs);     // bitwise intersection
    BitBoard    operator^(BitBoard const &rhs);     // bitwise exclusive or
    BitBoard    operator~(void);                    // bitwise complement
    BitBoard    operator>>(uint16_t shift) const;   // bitwise right shift
    BitBoard    operator<<(uint16_t shift) const;   // bitwise left shift

    /* assignment operator overload */
    BitBoard    &operator|=(BitBoard const &rhs);
    BitBoard    &operator&=(BitBoard const &rhs);
    BitBoard    &operator^=(BitBoard const &rhs);
    // TODO : add >>= and <<= operators

    /* member access operator overload */
    uint64_t    &operator[](uint8_t i);

    std::array<uint64_t, N>         values;
    static std::array<int16_t, D>   shifts;
    static BitBoard                 full_mask;
    static BitBoard                 empty_mask;

// private:

};

std::ostream	&operator<<(std::ostream &os, BitBoard const &bitboard);

BitBoard    dilation(BitBoard const &rhs, uint8_t amount); // see 4.3 at https://eprints.qut.edu.au/85005/1/__staffhome.qut.edu.au_staffgroupm%24_meaton_Desktop_bits-7.pdf
// BitBoard    erosion(BitBoard const &rhs, uint8_t amount);

#endif

/*
    +--19x19 BitBoard-----------------------+    +--Full Masks------------------------------------------------------+--------------------+
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |    |                              binary                              |    hexadecimal     |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |    +------------------------------------------------------------------+--------------------+
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |    | 1111111111111111111111111111111111111111111111111111111111111111 | 0xFFFFFFFFFFFFFFFF |
    | 1 1 1 1 1 1 1                              | 1111111111111111111111111111111111111111111111111111111111111111 | 0xFFFFFFFFFFFFFFFF |
                    1 1 1 1 1 1 1 1 1 1 1 1 |    | 1111111111111111111111111111111111111111111111111111111111111111 | 0xFFFFFFFFFFFFFFFF |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |    | 1111111111111111111111111111111111111111111111111111111111111111 | 0xFFFFFFFFFFFFFFFF |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |    | 1111111111111111111111111111111111111111111111111111111111111111 | 0xFFFFFFFFFFFFFFFF |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1                | 1111111111111111111111111111111111111111100000000000000000000000 | 0xFFFFFFFFFF800000 |
                                  1 1 1 1 1 |    +------------------------------------------------------------------+--------------------+
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1
          1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1
                      1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
                                      1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    +---------------------------------------+
    BitBoard without separating bits.

    +--Shifts---+---------------+-----+
    | direction |     value     | idx |        [indices]         [shifts]
    +-----------+---------------+-----+
    |     N     |      -19      |  0  |        7   0   1       -20 -19 -18
    |     S     |       19      |  4  |          ↖︎ ↑ ↗            ↖︎ ↑ ↗
    |     E     |        1      |  2  |        6 ← ◇ → 2       -1 ← ◇ → +1
    |     W     |       -1      |  6  |          ↙ ↓ ↘︎            ↙ ↓ ↘︎
    |    N-E    |      -18      |  1  |        5   4   3       +18 +19 +20
    |    S-E    |       20      |  3  |
    |    N-W    |      -20      |  7  |
    |    S-W    |       18      |  5  |
    +-----------+---------------+-----+

*/

/*
+--19x19 BitBoard-------------------------+   +--Full Masks------------------------------------------------------+--------------------+
| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |   |                              binary                              |    hexadecimal     |
| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |   +------------------------------------------------------------------+--------------------+
| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |   | 1111111111111111111011111111111111111110111111111111111111101111 | 0xFFFFEFFFFEFFFFEF |
| 1 1 1 1                                     | 1111111111111110111111111111111111101111111111111111111011111111 | 0xFFFEFFFFEFFFFEFF |
          1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |   | 1111111111101111111111111111111011111111111111111110111111111111 | 0xFFEFFFFEFFFFEFFF |
| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |   | 1111111011111111111111111110111111111111111111101111111111111111 | 0xFEFFFFEFFFFEFFFF |
| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |   | 1110111111111111111111101111111111111111111011111111111111111110 | 0xEFFFFEFFFFEFFFFE |
| 1 1 1 1 1 1 1 1                             | 1111111111111111111011111111111111111110111111111111111111100000 | 0xFFFFEFFFFEFFFFE0 |
                  1 1 1 1 1 1 1 1 1 1 1 0 |   +------------------------------------------------------------------+--------------------+
| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |
| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |
| 1 1 1 1 1 1 1 1 1 1 1 1
                          1 1 1 1 1 1 1 0 |
| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |
| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |
| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1
                                  1 1 1 0 |
| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |
| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |
| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |

| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |
| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |
| 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 |
+-----------------------------------------+
On this BitBoard representation of a 19x19 board we use separating bits to
help us in our algorithms down the line. As we can play in 4 axes each with
2 directions (8 directions total) we will declare a variable to align by
right shifting the values for all directions.

+--Shifts---+---------------+-----+
| direction |     value     | idx |        [indices]         [shifts]
+-----------+---------------+-----+
|     N     |      -20      |  0  |        7   0   1       -21 -20 -19
|     S     |       20      |  4  |          ↖︎ ↑ ↗            ↖︎ ↑ ↗
|     E     |        1      |  2  |        6 ← ◇ → 2       -1 ← ◇ → +1
|     W     |       -1      |  6  |          ↙ ↓ ↘︎            ↙ ↓ ↘︎
|    N-E    |      -19      |  1  |        5   4   3       +19 +20 +21
|    S-E    |       21      |  3  |
|    N-W    |      -21      |  7  |
|    S-W    |       19      |  5  |
+-----------+---------------+-----+
*/
