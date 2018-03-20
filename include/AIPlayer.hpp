#ifndef AIPLAYER_HPP
# define AIPLAYER_HPP

# include <iostream>
# include <Eigen/Dense>
# include "BitBoard.hpp"

# define SIZE 361   // the number of cells on the board (19*19)
# define S 3        // the number of states
# define INF 2147483647

namespace TranspositionTable {
    // enum flag {
    //     exact,
    //     lowerbound,
    //     upperbound,
    //     beta,
    //     alpha
    // };

    /* initialization of Zobrist Hashing */
    std::array<std::array<uint64_t, S>, SIZE>   _init_zobrist_table(void) {
        std::array<std::array<uint64_t, S>, SIZE> table;
        std::srand(std::time(nullptr));
        for (int n = 0; n < SIZE; n++) {
            for (int s = 0; s < S; s++)
                table[n][s] = std::rand();
        }
        return (table);
    }
    const std::array<std::array<uint64_t, S>, SIZE> _table = _init_zobrist_table();
    /* the key the hashing function will use */
    struct Key {
        BitBoard    p1;
        BitBoard    p2;

        bool operator==(const Key &other) const {
            return (p1 == other.p1 && p2 == other.p2);
        }
    };
    /* the hashing function that std::unordered_map will use */
    struct KeyHash {
        uint64_t operator()(const Key &k) const {
            uint64_t    hash = 0;
            for (int n = 0; n < SIZE; n++)
                hash ^= _table[n][(k.p1[n] == 0 ? (k.p2[n] == 0 ? 0 : 2) : 1)];
            return (hash);
        }
    };
}

typedef struct  s_node {
    BitBoard        player;
    BitBoard        player_forbidden;
    BitBoard        opponent;
    BitBoard        opponent_forbidden;
    uint8_t         pid;
    uint8_t         player_pairs_captured;
    uint8_t         opponent_pairs_captured;
}               t_node;

class AIPlayer {

public:
    AIPlayer(void);
    AIPlayer(AIPlayer const &src);
    ~AIPlayer(void);
    AIPlayer    &operator=(AIPlayer const&rhs);

    Eigen::Array2i  minmax(t_node node, int depth, int player);
    // int         negamax(t_node node, int depth, int player); /* Need a specific heuristic for this one */

    // int         mtdf();
    int             iterativedeepening(t_node node, int maxdepth);
    

private:
    TranspositionTable::unordered_map<Key, int, KeyHash>    TT_lowerbound;
    TranspositionTable::unordered_map<Key, int, KeyHash>    TT_upperbound;
    // int          alphabetawithmemory();
    BitBoard        get_moves(BitBoard const& player, BitBoard const& opponent,
                    BitBoard const& player_forbidden, int player_pairs_captured,
                    int opponent_pairs_captured);
    t_node          simulate_move(t_node const &node, int i);
    int32_t         score_function(t_node const &node, uint8_t depth);
    int32_t         player_score(t_node const &node, uint8_t depth);
    int32_t         opponent_score(t_node const &node, uint8_t depth);
    bool            check_end(t_node const& node);

    // t_ret        max(t_ret const& a, t_ret const& b) { return (a.score > b.score ? a : b); };
    // t_ret        min(t_ret const& a, t_ret const& b) { return (a.score < b.score ? a : b); };
    int             max(int const& a, t_ret const& b)   { return (a > b.score ? a : b.score); };
    int             min(int const& a, t_ret const& b)   { return (a < b.score ? a : b.score); };
    int             max(int const& a, int const& b)     { return (a > b ? a : b); };
    int             min(int const& a, int const& b)     { return (a < b ? a : b); };

};

#endif