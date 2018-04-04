#ifndef AIPLAYER_HPP
# define AIPLAYER_HPP

# include <iostream>
# include <unordered_map>
# include <array>
# include <vector>
# include <string>
# include "BitBoard.hpp"
# include "ZobristTable.hpp"

# define INF 2147483647
# define EXPLORATION_THRESHOLD 3

class Player;

typedef struct  s_node {
    BitBoard        player;
    BitBoard        opponent;
    uint8_t         cid;
    uint8_t         player_pairs_captured;
    uint8_t         opponent_pairs_captured;
    uint16_t        move; // NEW
}               t_node;

typedef struct  s_ret {
    int         score;
    int         p;

    bool operator<(s_ret const& b) const { return (score < b.score); }
    bool operator>(s_ret const& b) const { return (score > b.score); }
}               t_ret;

typedef struct  s_move {
    int         eval;
    int         p;
    t_node      node;
}               t_move;

namespace verbose {
    enum verbose {
        quiet,
        normal,
        debug
    };
};

class AIPlayer {

public:
    AIPlayer(int depth, uint8_t verbose = verbose::quiet);
    AIPlayer(AIPlayer const &src);
    virtual ~AIPlayer() {};
    AIPlayer    &operator=(AIPlayer const &);

    int         get_depth(void) const { return (_depth); };
    uint8_t     get_verbose(void) const { return (_verbose); };
    int32_t     score_function(t_node const &node, uint8_t depth);

    int             nbnode; // DEBUG
    int             nbleaf; // DEBUG

    virtual t_ret const operator()(t_node root) = 0;

protected:
    std::unordered_map<ZobristTable::Key, t_stored, ZobristTable::KeyHash>  _TT;
    int                                                                     _depth;
    uint8_t                                                                 _verbose;
    std::string                                                             _debug_string;

    std::vector<t_move> move_generation(t_node const& node, int depth);

    t_node              create_child(t_node const &node, int i);
    // int32_t             score_function(t_node const &node, uint8_t depth);
    int32_t             evaluation_function(t_node const &node, uint8_t depth);

    bool                checkEnd(t_node const& node);

    // void                debug_append_explored(int score, int i, int depth);
    // void                debug_search(t_ret const& ret);

    t_ret               max(t_ret const& a, t_ret const& b) { return (a.score > b.score ? a : b); };
    t_ret               min(t_ret const& a, t_ret const& b) { return (a.score < b.score ? a : b); };
    int                 max(int const& a, t_ret const& b)   { return (a > b.score ? a : b.score); };
    int                 min(int const& a, t_ret const& b)   { return (a < b.score ? a : b.score); };
    int                 max(int const& a, int const& b)     { return (a > b ? a : b); };
    int                 min(int const& a, int const& b)     { return (a < b ? a : b); };


};

t_node              create_node(Player const& player, Player const& opponent);

BitBoard            get_moves(BitBoard const& player, BitBoard const& opponent, BitBoard const& player_forbidden, int player_pairs_captured, int opponent_pairs_captured);
bool                sort_ascending(t_move const& a, t_move const& b);
bool                sort_descending(t_move const& a, t_move const& b);

int                 range(int const& x, int const& min, int const& max);
int64_t             range(int64_t const& x, int64_t const& min, int64_t const& max);

#endif
