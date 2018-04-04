#include "GameEngine.hpp"
#include "Player.hpp"

GameEngine::GameEngine(void) {
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

uint8_t check_end(BitBoard const& p1, BitBoard const& p2, uint8_t const& p1_pairs_captured, uint8_t const& p2_pairs_captured, uint16_t const& p) {
    BitBoard    move;
    move.write(p);

    /* if player captured 5 pairs */
    if (p1_pairs_captured >= 5)
        return (end::player_win);
    /* if opponent has a 5 alignment and move is not responsible (ex : white has breakable 5 aligned and black doesn't play the capture) */
    if (detect_five_aligned(p2) && (highlight_five_aligned(p2) & move).is_empty())
        return (end::opponent_win);

    /* if player has a 5 alignment */
    if (detect_five_aligned(p1)) {
        /* and move is not responsible */
        if ((highlight_five_aligned(p1) & move).is_empty())
            return (end::player_win);
        /* and move is responsible */
        else {
            /* and opponent already have a 5 alignment */
            if (detect_five_aligned(p2))
                return (end::opponent_win);
            /* and opponent can break the player's alignment next turn, continue */
            if (!pair_capture_breaking_five_detector(p2, p1).is_empty())
                return (end::none);
            /* and opponent can total 5 pairs captured next turn, continue */
            if (!win_by_capture_detector(p2, p1, p2_pairs_captured).is_empty())
                return (end::none);
            /* and opponent can't touch this */
            return (end::player_win);
        }
    }
    /* if there is no more room to play */
    if (((p1 | p2) ^ BitBoard::full).is_empty())
        return (end::draw);
    return (end::none);
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

void    GameEngine::update_dynamic_pattern_weights(void) {
    if (this->_history.size() > 2) {
        int         p_curr;
        int         p_last;
        int         delta;
        t_action    action = this->_history.back();
        t_action    action_prev = *std::prev(this->_history.end(), 3); // get the 3rd action from the end

        for (int i = 0; i < 8; ++i) {
            p_curr = pattern_detector(action.p1_last, action.p2_last, BitBoard::patterns[i]).set_count();
            p_last = pattern_detector(action_prev.p1_last, action_prev.p2_last, BitBoard::patterns[i]).set_count();
            delta = (p_curr - p_last);
            (action.pid == 1 ? BitBoard::p1_pattern_weights[i] : BitBoard::p2_pattern_weights[i]) += (delta > 0 ? delta : 0) * 10;
        }
    }
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
    for (uint8_t d = 0; d < 8; d++) {
        tmp = bitboard;
        for (int n = 1; !tmp.is_empty(); ++n) {
            tmp = (d > 0 && d < 4 ? tmp & ~BitBoard::border_right : (d > 4 && d < 8 ? tmp & ~BitBoard::border_left : tmp));
            tmp &= tmp.shifted(d);
            if (n >= 4) {
                for (uint8_t v = 0; v < 6; v++) {
                    if (tmp.values[v]) {
                        while (p++ < BITS) if (0x8000000000000000 & (tmp.values[v] << p)) break;
                        off = BitBoard::shifts[(d < 4 ? d + 4 : d - 4)] * n;
                        pos << (BITS * v + p + off) % 19,
                               (BITS * v + p + off) / 19,
                               (BITS * v + p) % 19,
                               (BITS * v + p) / 19;
                        return (pos);
                    }
                }
            }
        }
    }
    return (pos);
}
