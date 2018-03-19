#include "Computer.hpp"
#include <cstdlib> // TMP
#include <thread> // TMP

Computer::Computer(GameEngine *game_engine, GraphicalInterface *gui, uint8_t id) : Player(game_engine, gui, id) {
    std::srand(std::time(nullptr));
    this->type = 1;
}

Computer::Computer(Computer const &src) : Player(src) {
    *this = src;
}

Computer::~Computer(void) {
}

Computer	&Computer::operator=(Computer const &src) {
    this->_game_engine = src.get_game_engine();
    this->_id = src.get_id();
    return (*this);
}

bool        Computer::play(Player *other) {
    std::chrono::steady_clock::time_point   action_beg;
    t_action                                action;
    Eigen::Array2i                          pos;
    t_node                                  root = create_node(*this, *other);

    action_beg = std::chrono::steady_clock::now();

    // action.pos = alphabeta_pruning(&node, -INF, INF, 3);
    // action.pos = iterative_deepening(&node, 3);

    // int16_t p = mtdf(&node, 0, 3).p;

    // action.pos = iterativeDeepening(node, 10);
    // AlphaBeta   alphabeta(4, false, 500);
    // int16_t p = alphabeta(node, -INF, INF, 5).p;

    t_ret ret = alphaBetaWithMemory(root, -INF, INF, 6);
    action.pos = { ret.p / 19, ret.p % 19 };
    this->_gui->explored_moves_tmp = moves_to_explore(root.player, root.opponent, root.player_forbidden, root.player_pairs_captured, root.opponent_pairs_captured);

    // std::this_thread::sleep_for(std::chrono::milliseconds(100 + std::rand() % 900));
   // action.pos = {std::rand() % 19, std::rand() % 19}; // TMP
    action.duration = std::chrono::steady_clock::now() - action_beg;
    action.timepoint = std::chrono::steady_clock::now() - this->_game_engine->get_initial_timepoint();

    action.id = this->_game_engine->get_history_size() + 1;
    action.p1_last = this->board;
    action.p2_last = other->board;
    action.pid = this->_id;
    action.ppc = this->_pairs_captured;
    this->_game_engine->update_game_state(action, this, other);
    return true;
}
