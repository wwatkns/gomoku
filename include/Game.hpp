#ifndef GAME_HPP
# define GAME_HPP

# include <vector>
# include <string>
# include "Player.hpp"
# include "Human.hpp"
# include "Computer.hpp"
# include "GameEngine.hpp"
# include "GraphicalInterface.hpp"

class Game {

public:
    Game(void);
    Game(Game const &src);
    ~Game(void);
    Game	&operator=(Game const &rhs);

    void    loop(void);
    void    restart(void);
    void    end(void) const;

    /* Getters */
    Player      *get_player_1(void) const;
    Player      *get_player_2(void) const;
    GameEngine  *get_game_engine(void) const;
    /* Setters */
    void        set_player_1(Player player);
    void        set_player_2(Player player);

private:
    Player              *_player_1;
    Player              *_player_2;
    Player              *_c_player;
    GameEngine          *_game_engine;
    GraphicalInterface  *_gui;

};

#endif
