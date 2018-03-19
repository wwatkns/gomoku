#include "GameEngine.hpp"
#include "Player.hpp"

GameEngine::GameEngine(void) : _game_turn(0) {
    this->grid = Eigen::ArrayXXi::Constant(BOARD_COLS, BOARD_ROWS, state::free);
    this->_initial_timepoint = std::chrono::steady_clock::now();
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

// doen't work as intended
uint8_t check_end(BitBoard const& p1, BitBoard const& p2, uint8_t const& p1_pairs_captured, uint8_t const& p2_pairs_captured, uint8_t const& pid) {
    static bool dp[2] = { false, false };
    bool *d = (pid == 1 ? &dp[0] : &dp[1]);

    if (p1_pairs_captured >= 5)
        return (1);
    /* if we detected a five alignment last turn and it's still here */
    if (*d && detect_five_aligned(p1))
        return (1);
    if (detect_five_aligned(p1)) {
        if (!highlight_win_capture_moves(p2, p1, p2_pairs_captured).is_empty() && !*d) { /* p2 wins next turn if he can play a move allowing him to capture for a total of 5 or more stones */
            *d = true;
            return (0);
        }
        if (!pair_capture_breaking_five_detector(p2, p1).is_empty()) { /* game continue by capturing stones that will break the 5 alignment */
            *d = true;
            return (0);
        }
        return (1);
    }
    else
        *d = (*d ? false : *d);
    if (((p1 | p2) ^ BitBoard::full).is_empty())
        return (2);
    return (0);
}

void    GameEngine::update_game_state(t_action &action, Player *p1, Player *p2) {
    p1->board.write(action.pos[1], action.pos[0]);
    BitBoard captured = highlight_captured_stones(p1->board, p2->board, (action.pos[0] * 19 + action.pos[1]) );
    if (!captured.is_empty()) {
        p1->set_pairs_captured(p1->get_pairs_captured() + captured.set_count() / 2);
        p2->board &= ~captured;
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
