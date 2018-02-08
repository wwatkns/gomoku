#ifndef GAME_HPP
# define GAME_HPP

# include <vector>
# include <string>
# include "Player.hpp"
# include "Human.hpp"
# include "Computer.hpp"
# include "GameEngine.hpp"

class Game {

public:
    Game(void);
    Game(Game const &src);
    ~Game(void);
    Game	&operator=(Game const &rhs);

    void    loop(void);
    void    end(void) const;

    /*
        Constructor:
            -> instanciate a `GameEngine`
            -> instanciate an `InputHandler`
            -> use method "assign" from `InputHandler` to assign players ( Human / Computer )
            -> instanciate `player_1` (will always be black, giving it a reference to the instance of GameEngine)
            -> instanciate `player_2` (will always be white, giving it a reference to the instance of GameEngine)

        Loop:
            -> use method "play" from current player (which returns an `Action`), it is not the same implementation for `Human` and `Computer`
            -> use method "check_win" from `GameEngine` to check if the game is won
            -> use method "switch_player" from `GameEngine` to switch to the next player

    */

    /* Getters */
    Player      *get_player_1(void) const;
    Player      *get_player_2(void) const;
    GameEngine  *get_game_engine(void) const;
    /* Setters */
    void        set_player_1(Player player);
    void        set_player_2(Player player);


private:
    Player      *_player_1;
    Player      *_player_2;
    Player      *_c_player; /* a pointer to the current player */
    GameEngine  *_game_engine;

};

#endif
