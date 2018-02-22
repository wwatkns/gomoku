#include "MinMax.hpp"
#include "Player.hpp"
#include "GameEngine.hpp"

MinMax::MinMax(GameEngine *game_engine, uint8_t depth) : _game_engine(game_engine), _depth(depth) {
}

MinMax::MinMax(MinMax const &src) {
    *this = src;
}

MinMax::~MinMax(void) {
}

MinMax	&MinMax::operator=(MinMax const &src) {
    return (*this);
}

Eigen::Array2i  MinMax::minmax(Eigen::ArrayXXi grid, Player *player) {
    std::vector<Eigen::Array2i> open_moves;
    t_state                     game_state;
    uint32_t                    score;
    uint32_t                    min;
    int32_t                     tmp;
    Eigen::Array2i              pos;

    game_state.grid = grid;
    game_state.current_player_id = player->get_id();
    game_state.p1_pairs_captured = player->get_pairs_captured(); // TODO : do better
    game_state.p2_pairs_captured = player->get_pairs_captured(); // TODO : do better

    // open_moves = this->_get_open_moves(game_state);
    open_moves = this->_get_around_stone_moves(game_state);
    std::cout << "num moves : " << open_moves.size() << std::endl;

    min = std::numeric_limits<unsigned int>::max();
    pos = open_moves[0];

    for (std::vector<Eigen::Array2i>::iterator it = open_moves.begin(); it != open_moves.end(); ++it) {
        /* save value */
        tmp = game_state.grid((*it)[0], (*it)[1]);
        /* update game state */
        game_state.grid((*it)[0], (*it)[1]) = (game_state.current_player_id == 1 ? -1 : 1);
        game_state.current_player_id = (game_state.current_player_id == 1 ? 2 : 1);
        /* call min */
        score = this->_min(game_state, 1);
        if (score > min) {
            min = score;
            pos = *it;
        }
        /* reset grid */
        game_state.grid((*it)[0], (*it)[1]) = tmp;
    }
    return pos;
}

int32_t     MinMax::_min(t_state game_state, uint8_t current_depth) {
    std::vector<Eigen::Array2i> open_moves = this->_get_around_stone_moves(game_state);
    // std::vector<Eigen::Array2i> open_moves = this->_get_open_moves(game_state);
    uint32_t                    score;
    uint32_t                    max;
    int32_t                     tmp;

    uint8_t cpc = (game_state.current_player_id == 1 ? game_state.p1_pairs_captured : game_state.p2_pairs_captured);
    if (current_depth == this->_depth || this->_game_engine->check_end(cpc)) {
        return this->_score(game_state);
    }
    max = 0;
    for (std::vector<Eigen::Array2i>::iterator it = open_moves.begin(); it != open_moves.end(); ++it) {
        /* save value */
        tmp = game_state.grid((*it)[0], (*it)[1]);
        /* update game state */
        game_state.grid((*it)[0], (*it)[1]) = (game_state.current_player_id == 1 ? -1 : 1);
        game_state.current_player_id = (game_state.current_player_id == 1 ? 2 : 1);
        /* call max */
        score = this->_max(game_state, current_depth+1);
        max = (score > max ? score : max);
        /* reset grid */
        game_state.grid((*it)[0], (*it)[1]) = tmp;
    }
    return (max);
}

int32_t     MinMax::_max(t_state game_state, uint8_t current_depth) {
    std::vector<Eigen::Array2i> open_moves = this->_get_around_stone_moves(game_state);
    // std::vector<Eigen::Array2i> open_moves = this->_get_open_moves(game_state);
    uint32_t                    score;
    uint32_t                    min;
    int32_t                     tmp;

    uint8_t cpc = (game_state.current_player_id == 1 ? game_state.p1_pairs_captured : game_state.p2_pairs_captured);
    if (current_depth == this->_depth || this->_game_engine->check_end(cpc)) {
        return this->_score(game_state);
    }
    min = std::numeric_limits<unsigned int>::max();
    for (std::vector<Eigen::Array2i>::iterator it = open_moves.begin(); it != open_moves.end(); ++it) {
        /* save value */
        tmp = game_state.grid((*it)[0], (*it)[1]);
        /* update game state */
        game_state.grid((*it)[0], (*it)[1]) = (game_state.current_player_id == 1 ? -1 : 1);
        game_state.current_player_id = (game_state.current_player_id == 1 ? 2 : 1);
        /* call min */
        score = this->_min(game_state, current_depth+1);
        min = (score < min ? score : min);
        /* reset grid */
        game_state.grid((*it)[0], (*it)[1]) = tmp;
    }
    return (min);
}

int32_t     MinMax::_score(t_state game_state) {
    /*  We evaluate the current state of the board and add the scores of the patterns we detect in it :
        open three      : good  ( -o-oo- is better than -ooo- ), 3 variants
        open four       : win
        close three     : ok
        close four      : good (but win if we have more than 1)
        three-four      : win
        double-four     : win
        pair captures
    */
    return 1;
}

std::vector<Eigen::Array2i> MinMax::_get_around_stone_moves(t_state game_state) {
    std::vector<Eigen::Array2i> open_moves;

    for (uint32_t j = 0; j < game_state.grid.cols(); j++) {
        for (uint32_t i = 0; i < game_state.grid.rows(); i++) {
            /* absolute to get -1 and 1 */
            if (std::abs(game_state.grid(i, j)) == 1) { /* should use hashmaps to avoid duplicate as we can have position that are already present if stones are side to side */
                if (i >  0 && std::abs(game_state.grid(i-1, j)) != 1) open_moves.push_back({ i-1, j });
                if (j >  0 && std::abs(game_state.grid(i, j-1)) != 1) open_moves.push_back({ i, j-1 });
                if (i < 18 && std::abs(game_state.grid(i+1, j)) != 1) open_moves.push_back({ i+1, j });
                if (j < 18 && std::abs(game_state.grid(i, j+1)) != 1) open_moves.push_back({ i, j+1 });
                if (i >  0 && j >  0 && std::abs(game_state.grid(i-1, j-1)) != 1) open_moves.push_back({ i-1, j-1 });
                if (i < 18 && j >  0 && std::abs(game_state.grid(i+1, j-1)) != 1) open_moves.push_back({ i+1, j-1 });
                if (i >  0 && j < 18 && std::abs(game_state.grid(i-1, j+1)) != 1) open_moves.push_back({ i-1, j+1 });
                if (i < 18 && j < 18 && std::abs(game_state.grid(i+1, j+1)) != 1) open_moves.push_back({ i+1, j+1 });
            }
        }
    }
    return open_moves;
}

std::vector<Eigen::Array2i> MinMax::_get_open_moves(t_state game_state) {
    std::vector<Eigen::Array2i> open_moves;

    for (uint32_t i = 0; i < game_state.grid.size(); i++) {
        if (*(game_state.grid.data()+i) == 0 || *(game_state.grid.data()+i) == (game_state.current_player_id == 1 ? 10 : -10))
            open_moves.push_back((Eigen::Array2i){ i % game_state.grid.rows(), i / game_state.grid.rows() });
    }
    return open_moves;
}
