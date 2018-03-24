#ifndef MTDF_HPP
# define MTDF_HPP

# include <string>
# include <set>
# include <vector>
# include <algorithm>
# include <iostream>
# include <Eigen/Dense>
# include <stdio.h>
# include "BitBoard.hpp"
# include "ZobristTable.hpp"

# define INF 2147483647
# define EXPLORATION_THRESHOLD 5

class Player;

typedef struct  s_node {
    BitBoard        player;
    BitBoard        opponent;
    uint8_t         cid;
    uint8_t         player_pairs_captured;
    uint8_t         opponent_pairs_captured;
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

t_node          create_node(Player const& player, Player const& opponent);

bool            check_end(t_node const& node);

BitBoard        moves_to_explore(BitBoard const& player, BitBoard const& opponent, BitBoard const& player_forbidden, int player_pairs_captured, int opponent_pairs_captured);
// t_ret           alphaBetaWithMemory(t_node node, int alpha, int beta, int depth);
// t_ret           alphaBetaMin(t_node node, int alpha, int beta, int depth);
// t_ret           alphaBetaMax(t_node node, int alpha, int beta, int depth);

// bool            times_up(std::chrono::steady_clock::time_point start, uint32_t limit);

// t_ret           mtdf(t_node *root, int32_t firstguess, int8_t depth);

int32_t         evaluation_function(t_node const &node, uint8_t depth); // WIP
int32_t         score_function(t_node const &node, uint8_t depth);

// t_node          simulate_move(t_node const &node, int i);

// int32_t         min_val(int32_t const &a, int32_t const &b);
// int32_t         max_val(int32_t const &a, int32_t const &b);

// void            TT_store(t_node const &node, int32_t best, int32_t alpha, int32_t beta, int8_t depth, int flag);
// int32_t         TT_lookup(t_node const &node, int32_t alpha, int32_t beta, int8_t depth);

// t_ret           iterativeDeepening(t_node root, int max_depth);

t_ret           max(t_ret const& a, t_ret const& b);
t_ret           min(t_ret const& a, t_ret const& b);
int             max(int const& a, t_ret const& b);
int             min(int const& a, t_ret const& b);
int             max(int const& a, int const& b);
int             min(int const& a, int const& b);

int             range(int const& x, int const& min, int const& max);
int64_t         range(int64_t const& x, int64_t const& min, int64_t const& max);



namespace verbose {
    enum verbose {
        quiet,
        normal,
        debug
    };
};

/* comparison function for multisets */
struct retmaxcmp {
    bool operator() (t_ret const& lhs, t_ret const& rhs) const {
        return (lhs.score > rhs.score);
    }
};
struct retmincmp {
    bool operator() (t_ret const& lhs, t_ret const& rhs) const {
        return (lhs.score < rhs.score);
    }
};

/* Alpha-Beta pruning with iterative deepening and root move ordering */
class AlphaBetaWithIterativeDeepening {

public:
    AlphaBetaWithIterativeDeepening(int max_depth, int time_limit = 500, uint8_t pid = 1, uint8_t verbose = verbose::quiet);
    AlphaBetaWithIterativeDeepening(AlphaBetaWithIterativeDeepening const &src);
    ~AlphaBetaWithIterativeDeepening(void);
    AlphaBetaWithIterativeDeepening	&operator=(AlphaBetaWithIterativeDeepening const &rhs);

    int         get_max_depth(void) const { return (_max_depth); };
    int         get_search_limit_ms(void) const { return (_search_limit_ms); };

    t_ret const operator()(t_node root);

    bool    search_stopped;

private:
    int                                     _max_depth;
    int                                     _current_max_depth;
    std::chrono::steady_clock::time_point   _search_start;
    int                                     _search_limit_ms;
    uint8_t                                 _pid;
    uint8_t                                 _verbose;
    std::string                             _debug_string;
    // std::multiset<t_ret, retmaxcmp>         _ordered_root_moves;
    std::vector<t_move>                     _root_moves;

    /* analytics */
    int                                     _n_explored_nodes;
    int                                     _n_retreived_nodes;
    int                                     _n_stored_nodes;

    t_ret const                             _mtdf(t_node root, int firstguess, int depth);
    t_ret                                   _root_max(t_node node, int alpha, int beta, int depth);
    t_ret                                   _max(t_node node, int alpha, int beta, int depth);
    t_ret                                   _min(t_node node, int alpha, int beta, int depth);

    std::vector<t_move>                     _move_generation(t_node const& node, int depth);

    t_node                                  _create_child(t_node const& parent, int m);
    bool                                    _times_up(void);
    int                                     _elapsed_ms(void);
    void                                    _debug_append_explored(int score, int i, int depth);
    void                                    _debug_search(t_ret const& ret);
};

/* Alpha-Beta pruning with iterative deepening and root move ordering */
class AlphaBetaWithMemory {

public:
    AlphaBetaWithMemory(int max_depth, int time_limit = 500, uint8_t pid = 1, uint8_t verbose = verbose::quiet);
    AlphaBetaWithMemory(AlphaBetaWithMemory const &src);
    ~AlphaBetaWithMemory(void);
    AlphaBetaWithMemory	&operator=(AlphaBetaWithMemory const &rhs);

    int         get_max_depth(void) const { return (_max_depth); };
    int         get_search_limit_ms(void) const { return (_search_limit_ms); };

    t_ret const operator()(t_node root);

    bool    search_stopped;

private:
    int                                     _max_depth;
    int                                     _current_max_depth;
    std::chrono::steady_clock::time_point   _search_start;
    int                                     _search_limit_ms;
    uint8_t                                 _pid;
    uint8_t                                 _verbose;
    std::string                             _debug_string;
    std::multiset<t_ret, retmaxcmp>         _ordered_root_moves;
    std::unordered_map<ZobristTable::Key, t_stored, ZobristTable::KeyHash>  _TT;

    /* analytics */
    int                                     _n_explored_nodes;
    int                                     _n_retreived_nodes;
    int                                     _n_stored_nodes;

    t_ret const                             _mtdf(t_node root, int firstguess, int depth);
    t_ret                                   _root_max(t_node node, int alpha, int beta, int depth);
    t_ret                                   _max(t_node node, int alpha, int beta, int depth);
    t_ret                                   _min(t_node node, int alpha, int beta, int depth);

    t_ret                                   _TT_lookup(t_node const& node, int alpha, int beta, int8_t depth);
    void                                    _TT_store(t_node const& node, t_ret best, int8_t depth, int8_t flag);

    t_node                                  _create_child(t_node const& parent, int m);
    bool                                    _times_up(void);
    int                                     _elapsed_ms(void);
    void                                    _debug_append_explored(int score, int i, int depth);
    void                                    _debug_search(t_ret const& ret);
};

#endif
