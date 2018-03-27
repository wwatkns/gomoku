#ifndef BITBOARD_HPP
# define BITBOARD_HPP

# include <iostream>
# include <array>
/* includes for debug purposes */
# include <bitset>
# include <string>
# include <sstream>

# define N 6
# define D 8
# define BITS 64

typedef struct  s_pattern {
    uint8_t     repr;   /* the pattern encoded in big-endian (ex : 01011000 for -O-OO-) */
    uint8_t     size;   /* the size of the pattern */
    uint8_t     dirs;   /* the number of directions (symmetric pattern like -OOO- need only 4 directions computed) */
    uint16_t    value;  /* the value associated with the pattern */
}               t_pattern;

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
    BitBoard	&operator=(uint64_t const &val);

    uint64_t    row(uint8_t i) const;                   // access row at index

    void        zeros(void);                            // set the bitboard values to zeros
    void        write(const uint64_t i);
    void        remove(const uint64_t i);
    void        write(const uint64_t x, const uint64_t y);            // set a bit to 1 on the bitboard at position x, y
    void        remove(const uint64_t x, const uint64_t y);           // set a bit to 0 on the bitboard at position x, y
    int         set_count(void) const;                  // return the number of bits set to 1 in bitboard
    bool        check_bit(const uint64_t i) const;
    bool        check_bit(const uint64_t x, const uint64_t y) const;  // check if the bit a position x, y is set to 1
    bool        is_empty(void) const;                   // check if all bits in the bitboard are set to 0
    void        broadcast_row(uint64_t line);           // copy the given row (first 19 bits) to all other rows
    BitBoard    neighbours(void) const;                 // returns the neighbouring cells

    int         leftmost_bit(void) const;
    int         rightmost_bit(void) const;

    BitBoard    shifted(const uint8_t dir, const uint8_t n = 1) const;
    BitBoard    shifted_inv(const uint8_t dir, const uint8_t n = 1) const;
    BitBoard    dilated(void) const;
    BitBoard    eroded(void) const;

    BitBoard    rotated_45(void);

    /* arithmetic (bitwise) operator overload */
    BitBoard    operator|(BitBoard const &rhs) const; // bitwise union
    BitBoard    operator&(BitBoard const &rhs) const; // bitwise intersection
    BitBoard    operator^(BitBoard const &rhs) const; // bitwise exclusive or
    BitBoard    operator~(void) const;                // bitwise complement
    BitBoard    operator>>(const int32_t shift) const;// bitwise right shift
    BitBoard    operator<<(const int32_t shift) const;// bitwise left shift

    /* assignment operator overload */
    BitBoard    &operator|=(BitBoard const &rhs);
    BitBoard    &operator&=(BitBoard const &rhs);
    BitBoard    &operator^=(BitBoard const &rhs);
    BitBoard    &operator<<=(const int32_t shift);
    BitBoard    &operator>>=(const int32_t shift);

    /* member access operator overload */
    uint64_t    operator[](const int i) const; // will return the i-th bit

    /* comparison operator overload */
    bool        operator==(BitBoard const &rhs) const;
    bool        operator!=(BitBoard const &rhs) const;

    std::array<uint64_t, N>                 values;
    static const std::array<int16_t, D>     shifts;
    static const std::array<t_pattern,8>    patterns;
    static const BitBoard                   full;
    static const BitBoard                   empty;
    static const BitBoard                   border_right;
    static const BitBoard                   border_left;
    static const BitBoard                   border_top;
    static const BitBoard                   border_bottom;

};

std::ostream	&operator<<(std::ostream &os, BitBoard const &bitboard);

BitBoard    get_threat_moves(BitBoard const &p1, BitBoard const &p2, int p2_pairs_captured);
BitBoard    get_winning_moves(BitBoard const &p1, BitBoard const &p2, int p1_pairs_captured, int p2_pairs_captured);

BitBoard    forbidden_detector(BitBoard const &p1, BitBoard const &p2);
BitBoard    future_pattern_detector(BitBoard const &p1, BitBoard const &p2, t_pattern const &pattern);
BitBoard    pattern_detector(BitBoard const &p1, BitBoard const &p2, t_pattern const &pattern);
BitBoard    pattern_detector_highlight_open(BitBoard const &p1, BitBoard const &p2, t_pattern const &pattern);

bool        detect_five_aligned(BitBoard const &bitboard);
BitBoard    highlight_five_aligned(BitBoard const &bitboard);

BitBoard    pair_capture_detector(BitBoard const &p1, BitBoard const &p2);      /* detect the positions leading to one or multiple captures */
BitBoard    highlight_captured_stones(BitBoard const &p1, BitBoard const &p2, int move);  /* return the board showing the stones captured */
BitBoard    pair_capture_detector_highlight(BitBoard const &p1, BitBoard const &p2); /* detect the positions leading to one or multiple captures but highlights the stones that will be captured */
BitBoard    pair_capture_breaking_five_detector(BitBoard const &p1, BitBoard const &p2);

BitBoard    win_by_capture_detector(BitBoard const &p1, BitBoard const &p2, int p1_pairs_captured); /* detect the positions that lead to an instant win by capture */
BitBoard    win_by_alignment_detector(BitBoard const &p1, BitBoard const &p2, BitBoard const& p1_forbidden, int p2_pairs_captured); /* detect the positions that lead to an instant win by five alignment (unbreakable) */

namespace direction {
    enum direction {
        north,
        north_east,
        east,
        south_east,
        south,
        south_west,
        west,
        north_west
    };
};

#endif

/*     +--19x19 BitBoard-----------------------+
     0 | . . . . . . . . . . . . . . . . . . . | 0
     1 | . . . . . . . . . . . . . . . . . . . | 1
     2 | . . . . . . . . . . . . . . . . . . . | 2
     3 | . . . . . . ./. . . . . . . . . . . . | 3
     4 | . . . . . . . . . . . . . . . . . . . | 4
     5 | . . . . . . . . . . . . . . . . . . . | 5
     6 | . . . . . . . . . . . . . ./. . . . . | 6
     7 | . . . . . . . . . . . . . . . . . . . | 7
     8 | . . . . . . . . . . . . . . . . . . . | 8
     9 | . . . . . . . . . . . . . . . . . . . | 9
    10 | . ./. . . . . . . . . . . . . . . . . | 10
    11 | . . . . . . . . . . . . . . . . . . . | 11
    12 | . . . . . . . . . . . . . . . . . . . | 12
    13 | . . . . . . . ./. . . . . . . . . . . | 13
    14 | . . . . . . . . . . . . . . . . . . . | 14
    15 | . . . . . . . . . . . . . . . . . . . | 15
    16 | . . . . . . . . . . . . . . . ./. . . | 16
    17 | . . . . . . . . . . . . . . . . . . . | 17
    18 | . . . . . . . . . . . . . . . . . . . | 18
       +---------------------------------------+
         0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8

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
    |    N-W    |      -20      |  7  |        inverse direction :
    |    S-W    |       18      |  5  |        (dir < 4 ? dir + 4 : dir - 4)
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

    Winning strategies :
        [instant]
         * five
         * 5 pairs captured
        [2 turns]
         * open-four
         * double-close-four
         * double-split-four (they include -OO-O-O-OO-)
         * split-four & close-four
        [4 turns]
         * close-four & open-three
         * close-four & open-split-three
         * split-four & open-three
         * split-four & open-split-three

    +---Patterns-(open)----------------------------------------------+----------+-------+------------+--------+----------------------+
    | num |  pattern |                     moves                     |  binary  |  hex  | directions | length |         names        |
    +-----+----------+-----------------------------------------------+----------+-------+------------+--------+----------------------+
    |  1  |   -OOO-  |                     -*OO-    -O*O-    -OO*-   | 01110000 |  0x70 |      4     |    5   |  open three          |
    |  2  |  -OO-O-  |                     -*O-O-   -O*-O-   -OO-*-  | 01101000 |  0x68 |      8     |    6   |  open split three    |  Moves Symbols :
    |  3  |  -OOOO-  |            -*OOO-   -O*OO-   -OO*O-   -OOO*-  | 01111000 |  0x78 |      4     |    6   |  open four           |  | O | p1 stones
    +---Patterns-(close)---------------------------------------------+----------+-------+------------+--------+----------------------+  | | | p2 stones
    |  4  |    OOO-  |                     |*OO-    |O*O-    |OO*-   | 11100000 |  0xE0 |      8     |    4   |  close three         |  | - | an open position
    |  5  |   OO-O-  |                     |*O-O-   |O*-O-   |OO-*-  | 11010000 |  0xD0 |      8     |    5   |  close split three 1 |  | ~ | any one of the above
    |  6  |   O-OO-  |                     |*-OO-   |O-*O-   |O-O*-  | 10110000 |  0xB0 |      8     |    5   |  close split three 2 |  | * | an open move that'll complete the pattern
    |  7  |   OOOO-  |            |*OOO-   |O*OO-   |OO*O-   |OOO*-  | 11110000 |  0xF0 |      8     |    5   |  close four          |
    +---Patterns-(other)---------------------------------------------+----------+-------+------------+--------+----------------------+
    |  8  |  -O-OOO  |            -*-OOO~  -O-*OO~  -O-O*O~  -O-OO*~ | 01011100 |  0x5C |      8     |    6   |  split four 1        |
    |  9  |  -OO-OO  |            -*O-OO~  -O*-OO~  -OO-*O~  -OO-O*~ | 01101100 |  0x6C |      8     |    6   |  split four 2        |
    | 10  |  -OOO-O  |            -*OO-O~  -O*O-O~  -OO*-O~  -OOO-*~ | 01110100 |  0x74 |      8     |    6   |  split four 3        |
    | 11  |   OOOOO  |   ~*OOOO~  ~O*OOO~  ~OO*OO~  ~OOO*O~  ~OOOO*~ | 11111000 |  0xF8 |      4     |    5   |  five                |
    +-----+----------+-----------------------------------------------+----------+-------+------------+--------+----------------------+

    -> pattern depict desired stone arrangements, as such the function pattern_detection returns a
     bitboard showing the open cells leading to such arrangements.
    -> patterns are encoded in 8 bits and in big-endian (from left to right) as shown in table above.
    -> patterns must have a length associated to them (is it still true ?).
    -> close pattern mean they have one extremity blocked by an enemy stone, both extremities closed
     are not checked as it will lead to nothing.
    -> asymmetric patterns such as -OO-O- are checked on 4 axis and both directions, so -O-OO- is
     checked as the same time.
    -> with this pattern encoding you can do (open & open), (open & close), (any & any), (open & any),
     (close & any) and the mirrors.


    +-16bit position storage---------+
    |     x    |    y    |    pad    |
    +----------+---------+-----------+-------------+
    | 1 1 1 1 1|1 1 1 1 1|0 0 0 0 0 0| binary mask |
    +----------+---------+-----------+-------------+
    |  0xF800  |  0x7C0  |           |    hex mask |
    +----------+---------+-----------+-------------+

    Encode :
     $ uint16_t pos = (x << 11) | (y << 6);
    Decode :
     $ x = pos & 0xF800;
     $ y = pos & 0x7C0;

     1001001111000000 is (18, 15)
*/
