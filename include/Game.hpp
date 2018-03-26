#ifndef GAME_HPP
# define GAME_HPP

# include <vector>
# include <string>
# include <boost/program_options.hpp>
# include "GraphicalInterface.hpp"
# include "BitBoard.hpp"
# include "Player.hpp"
# include "Human.hpp"
# include "Computer.hpp"
# include "GameEngine.hpp"
// # include "GraphicalInterface.hpp"
// # include "Chronometer.hpp"
# include <thread> // TMP

class Game {

public:
    Game(void);
    Game(Game const &src);
    ~Game(void);
    Game	&operator=(Game const &rhs);

    void    loop(void);
    bool    undo(void);
    void    newgame(void);
    void    restart(void);

    /* Getters */
    Player      *get_player_1(void) const { return _player_1; };
    Player      *get_player_2(void) const { return _player_2; };
    GameEngine  *get_game_engine(void) const { return _game_engine; };
    /* Setters */
    void        set_player_1(Player player) { _player_1 = &player; };
    void        set_player_2(Player player) { _player_2 = &player; };

private:
    Player              *_player_1;
    Player              *_player_2;
    Player              *_c_player;
    GameEngine          *_game_engine;
    GraphicalInterface  *_gui;
    std::string         _config;

    void                _debug_fps(void);
    void                _cap_framerate(uint32_t const &framerate);

};

#endif
