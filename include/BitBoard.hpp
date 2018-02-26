#ifndef BITBOARD_HPP
# define BITBOARD_HPP

# include <iostream>
# include <cstdlib>
# include <array>
/* includes for debug purposes */
# include <bitset>
# include <string>
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
    BitBoard    get_flipped(void);
    BitBoard    get_rotated_45(void);
    BitBoard    get_rotated_315(void);

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
    BitBoard    &operator<<=(uint16_t shift);
    BitBoard    &operator>>=(uint16_t shift);

    /* member access operator overload */
    uint64_t    &operator[](uint8_t i);

    std::array<uint64_t, N>         values;
    static std::array<int16_t, D>   shifts;
    static BitBoard                 full_mask;
    static BitBoard                 empty_mask;

// private:

};

std::ostream	&operator<<(std::ostream &os, BitBoard const &bitboard);

/* see 4.3 at https://eprints.qut.edu.au/85005/1/__staffhome.qut.edu.au_staffgroupm%24_meaton_Desktop_bits-7.pdf */
BitBoard    dilation(BitBoard const &bitboard);
BitBoard    erosion(BitBoard const &bitboard);
BitBoard    get_neighbours(BitBoard const &bitboard);

#endif

/*
    +--19x19 BitBoard-----------------------+    +--Full Masks------------------------------------------------------+--------------------+
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |    |                              binary                              |    hexadecimal     |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |    +------------------------------------------------------------------+--------------------+
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |    | 1111111111111111111111111111111111111111111111111111111111111111 | 0xFFFFFFFFFFFFFFFF |
    | 1 1 1 1 1 1 1                          6   | 1111111111111111111111111111111111111111111111111111111111111111 | 0xFFFFFFFFFFFFFFFF |
                    1 1 1 1 1 1 1 1 1 1 1 1 |    | 1111111111111111111111111111111111111111111111111111111111111111 | 0xFFFFFFFFFFFFFFFF |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |    | 1111111111111111111111111111111111111111111111111111111111111111 | 0xFFFFFFFFFFFFFFFF |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |    | 1111111111111111111111111111111111111111111111111111111111111111 | 0xFFFFFFFFFFFFFFFF |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1           13   | 1111111111111111111111111111111111111111100000000000000000000000 | 0xFFFFFFFFFF800000 |
                                  1 1 1 1 1 |    +------------------------------------------------------------------+--------------------+
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1                                    1
          1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1                        7
                      1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1       15
                                      1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    | 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 |
    +---------------------------------------+
    BitBoard without separating bits. We'll benchmark the performances of the
    algorithms once we have more stuff in place, and compare the version of the
    BitBoard with and without the separator bits.

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

    +--Normal Board------------------+      +--Flipped Board-----------------+
    | a8  b8  c8  d8  e8  f8  g8  h8 |      | a8  a7  a6  a5  a4  a3  a2  a1 |
    | a7  b7  c7  d7  e7  f7  g7  h7 |      | b8  b7  b6  b5  b4  b3  b2  b1 |
    | a6  b6  c6  d6  e6  f6  g6  h6 |      | c8  c7  c6  c5  c4  c3  c2  c1 |
    | a5  b5  c5  d5  e5  f5  g5  h5 |      | d8  d7  d6  d5  d4  d3  d2  d1 |
    | a4  b4  c4  d4  e4  f4  g4  h4 |      | e8  e7  e6  e5  e4  e3  e2  e1 |
    | a3  b3  c3  d3  e3  f3  g3  h3 |      | f8  f7  f6  f5  f4  f3  f2  f1 |
    | a2  b2  c2  d2  e2  f2  g2  h2 |      | g8  g7  g6  g5  g4  g3  g2  g1 |
    | a1  b1  c1  d1  e1  f1  g1  h1 |      | h8  h7  h6  h5  h4  h3  h2  h1 |
    +--------------------------------+      +--------------------------------+

    Rotated BitBoards allow for efficient checks for diagonals. The examples below
    are for a chess board, but the principle applies to all sizes.
    +--Rotated Board 45--------------+      +--Rotated Board 315-------------+
    | a8 |b1  c2  d3  e4  f5  g6  h7 |      | a8  b7  c6  d5  e4  f3  g2  h1 |
    | a7  b8 |c1  d2  e3  f4  g5  h6 |      | a7  b6  c5  d4  e3  f2  g1 |h8 |
    | a6  b7  c8 |d1  e2  f3  g4  h5 |      | a6  b5  c4  d3  e2  f1 |g8  h7 |
    | a5  b6  c7  d8 |e1  f2  g3  h4 |      | a5  b4  c3  d2  e1 |f8  g7  h6 |
    | a4  b5  c6  d7  e8 |f1  g2  h3 |      | a4  b3  c2  d1 |e8  f7  g6  h5 |
    | a3  b4  c5  d6  e7  f8 |g1  h2 |      | a3  b2  c1 |d8  e7  f6  g5  h4 |
    | a2  b3  c4  d5  e6  f7  g8 |h1 |      | a2  b1 |c8  d7  e6  f5  g4  h3 |
    | a1  b2  c3  d4  e5  f6  g7  h8 |      | a1 |b8  c7  d6  e5  f4  g3  h2 |
    +--------------------------------+      +--------------------------------+
                    a8                                      h8
                  a7  b8                                  g8  h7
                a6  b7  c8                              f8  g7  h6
              a5  b6  c7  d8                          e8  f7  g6  h5
            a4  b5  c6  d7  e8                      d8  e7  f6  g5  h4
          a3  b4  c5  d6  e7  f8                  c8  d7  e6  f5  g4  h3
        a2  b3  c4  d5  e6  f7  g8              b8  c7  d6  e5  f4  g3  h2
      a1  b2  c3  d4  e5  f6  g7  h8          a8  b7  c6  d5  e4  f3  g2  h1
        b1  c2  d3  e4  f5  g6  h7              a7  b6  c5  d4  e3  f2  g1
          c1  d2  e3  f4  g5  h6                  a6  b5  c4  d3  e2  f1
            d1  e2  f3  g4  h5                      a5  b4  c3  d2  e1
              e1  f2  g3  h4                          a4  b3  c2  d1
                f1  g2  h3                              a3  b2  c1
                  g1  h2                                  a2  b1
                    h1                                      a1
*/
