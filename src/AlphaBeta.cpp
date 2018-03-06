#include "AlphaBeta.hpp"

AlphaBeta::AlphaBeta(GameEngine *game_engine, uint8_t depth) : _game_engine(game_engine), _depth(depth) {
}

AlphaBeta::AlphaBeta(AlphaBeta const &src) {
    *this = src;
}

AlphaBeta::~AlphaBeta(void) {
}

AlphaBeta	&AlphaBeta::operator=(AlphaBeta const &src) {
    return (*this);
}

AlphaBeta::alphabeta_pruning() {

}

BitBoard::bitboard  AlphaBeta::_get_open_moves(t_state game_state) {

}

