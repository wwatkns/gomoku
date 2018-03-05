#include "GameEngine.hpp"
#include "Player.hpp"

GameEngine::GameEngine(void) : _game_turn(0) {
    this->grid = Eigen::ArrayXXi::Constant(BOARD_COLS, BOARD_ROWS, state::free);
    this->_initial_timepoint = std::chrono::steady_clock::now();
    // this->minmax = new MinMax(this, 2); // TODO : should put MinMax again in makefile
}

GameEngine::GameEngine(GameEngine const &src) {
  *this = src;
}

GameEngine::~GameEngine(void) {
}

GameEngine	&GameEngine::operator=(GameEngine const &src) {
    this->grid = src.grid;
    this->_history = src.get_history_copy();
    this->_initial_timepoint = src.get_initial_timepoint();
    return (*this);
}

bool    GameEngine::check_action(t_action const &action, Player const &p1, Player const &p2) {
    if ((p1.board|p2.board).check_bit(action.pos[1], action.pos[0]) == false &&
        (p1.board_forbidden.check_bit(action.pos[1], action.pos[0]) == false))
        return (true);
    return (false);
}

// uint8_t GameEngine::check_end(Player const &p1, Player const &p2) {
//     if (p1.get_pairs_captured() >= 5)
//         return (1);
//     BitBoard    pairs = pair_capture_detector(p2.board, p1.board);
//     if ((detect_five_aligned(p1.board) && !pairs.is_empty() && p2.get_pairs_captured() == 4) ||
//         (!(highlight_five_aligned(p1.board) & highlight_captured_stones(p2.board, p1.board, pairs)).is_empty()))
//         return (0);
//     if (((p1.board | p2.board) ^ BitBoard::full).is_empty() == true)
//         return (2);
//     return (0);
// }

uint8_t GameEngine::check_end(Player const &p1, Player const &p2) {
    if (p1.get_pairs_captured() >= 5)
        return (1);
    if (detect_five_aligned(p1.board)) {
        BitBoard    pairs = pair_capture_detector(p2.board, p1.board); // good
        /* p2 wins next turn by capturing a fifth pair even though p1 has 5 aligned */
        if (!pairs.is_empty() && p2.get_pairs_captured() == 4)
            return (0);
        /* game continue by capturing stones forming the 5 alignment */
        if ( detect_five_aligned(highlight_five_aligned(p1.board) ^ highlight_captured_stones(p1.board, p2.board ^ pairs, pairs)) == false )
            return (0);
        return (1);
    }
    if (((p1.board | p2.board) ^ BitBoard::full).is_empty() == true)
        return (2);
    return (0);
}

void    GameEngine::update_game_state(t_action &action, Player *p1, Player *p2) {
    BitBoard pairs = pair_capture_detector(p1->board, p2->board);
    p1->board.write(action.pos[1], action.pos[0]);
    if ((pairs & p1->board).is_empty() == false) {
        pairs = highlight_captured_stones(p2->board, p1->board, pairs);
        p1->set_pairs_captured(p1->get_pairs_captured() + pairs.set_count()/2);
        p2->board &= ~pairs;
    }
    p1->board_forbidden = forbidden_detector(p1->board, p2->board);
    p2->board_forbidden = forbidden_detector(p2->board, p1->board);
    GameEngine::update_grid(*p1, *p2);
    this->_history.push_back(action);
}

void GameEngine::update_grid_with_bitboard(BitBoard const &bitboard, int8_t const &state) {
    uint64_t    row;
    for (uint8_t y = 0; y < 19; y++) {
        row = bitboard.row(y);
        if (row) {
            for (uint8_t x = 0; x < 19; x++)
                if (row << x & 0x8000000000000000)
                    this->grid(y, x) = state;
        }
    }
}

void    GameEngine::update_grid(Player const &p1, Player const &p2) {
    uint8_t state = (p1.get_id() == 1 ? state::black : state::white);
    this->grid = Eigen::ArrayXXi::Zero(BOARD_COLS, BOARD_ROWS);
    GameEngine::update_grid_with_bitboard(p1.board,  state);
    GameEngine::update_grid_with_bitboard(p2.board, -state);
    GameEngine::update_grid_with_bitboard(p1.board_forbidden, state *  10);
    GameEngine::update_grid_with_bitboard(p2.board_forbidden, state * -10);
}

void    GameEngine::delete_last_action(Player *p1, Player *p2) {
    t_action    last;

    if (this->_history.size() > 0) {
        last = this->_history.back();
        if (last.pid == 1) {
            p1->board = last.p1_last;
            p2->board = last.p2_last;
            p1->set_pairs_captured(last.ppc);
        } else {
            p1->board = last.p2_last;
            p2->board = last.p1_last;
            p2->set_pairs_captured(last.ppc);
        }
        p1->board_forbidden = forbidden_detector(p1->board, p2->board);
        p2->board_forbidden = forbidden_detector(p2->board, p1->board);
        GameEngine::update_grid(*p1, *p2);
        this->_history.pop_back();
    }
}

Eigen::Array22i GameEngine::get_end_line(BitBoard const &bitboard) {
    BitBoard        tmp;
    Eigen::Array22i pos;
    int64_t p = -1;
    uint64_t off;

    pos << 0, 0, 0, 0;
    for (uint8_t i = 0; i < 4; i++) {
        tmp = bitboard;
        for(uint8_t n = 1; (tmp &= tmp.shifted(i)).is_empty() == false; ++n)
            if (n >= 4)
                for (uint8_t v = 0; v < 6; v++)
                    if (tmp.values[v]) {
                        while (p++ < BITS) if (0x8000000000000000 & (tmp.values[v] << p)) break;
                        off = BitBoard::shifts[(i < 4 ? i + 4 : i - 4)] * n;
                        pos << (BITS * v + p + off) % 19,
                               (BITS * v + p + off) / 19,
                               (BITS * v + p) % 19,
                               (BITS * v + p) / 19;
                        return (pos);
                    }
    }
    return (pos);
}

/* Setters */
void    GameEngine::inc_game_turn(void) {
    this->_game_turn++;
}
