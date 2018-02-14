#ifndef BUTTONSELECT_HPP
# define BUTTONSELECT_HPP

# include <iostream>
# include <string>
# include <vector>
# include <Eigen/Dense>
# include <SDL.h>
// # include <SDL_ttf.h>
// # include "FontHandler.hpp"
// # include "FontText.hpp"
# include "Button.hpp"

class ButtonSelect {

public:
    ButtonSelect(std::vector<Button*> buttons);
    ButtonSelect(ButtonSelect const &src);
    ~ButtonSelect(void);
    ButtonSelect	&operator=(ButtonSelect const &rhs);

    bool        get_state(void) const { return _state; };
    void        update_state(Eigen::Array2i *pos, bool mouse_press);
    void        render(SDL_Renderer *renderer);

private:
    std:vector<Button*> _buttons;
    uint32_t            _activated_button;

};

#endif
