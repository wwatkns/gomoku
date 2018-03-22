#include "Computer.hpp"

#include <cstdlib> // TMP
#include <thread> // TMP

Computer::Computer(GameEngine *game_engine, uint8_t id) : Player(game_engine, NULL, id) {
    std::srand(std::time(nullptr));
    this->type = 1;
    this->ai_algorithm = new AIPlayer;
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
    t_node                                  node;
    t_best                                  bestmove;

    node.player = this->board;
    node.opponent = other->board;
    node.player_forbidden = this->board_forbidden;
    node.opponent_forbidden = other->board_forbidden;
    node.pid = 1;//this->_id;
    node.player_pairs_captured = this->_pairs_captured;
    node.opponent_pairs_captured = other->get_pairs_captured();

    action_beg = std::chrono::steady_clock::now();

    /* AIPlayer test */
    bestmove = ai_algorithm->minmax(node, 4, 1);
    action.pos = (Eigen::Array2i){ bestmove.pos / 19, bestmove.pos % 19 };

    // action.pos = alphabeta_pruning(&node, -INF, INF, 3);
    // action.pos = iterative_deepening(&node, 3);

    // int16_t p = mtdf(&node, 0, 3).p;

    // std::cout << "--- player " << std::to_string(other->get_id()) << std::endl;
    // action.pos = iterative_deepening(&node, 10);
    // action.pos = mtdf();
    // action.pos = ft_alphaBetaWithMemory(&node, -INF, INF, 3).p;


    // t_ret g = { 0, 0 };
    // g = mtdf(&node, g, 5);
    // std::cout << "--- score: " << g.score << ", pos: (" << g.p / 19 << ", " << g.p % 19 << ")\n";
    // action.pos = { g.p / 19, g.p % 19 };
    
    // AlphaBeta   alphabeta(4, false, 500);
    // int16_t p = alphabeta(node, -INF, INF, 5).p;

    // int16_t p = alphaBetaWithMemory(node, -INF, INF, 4).p;
    // action.pos = { p / 19, p % 19 };
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
