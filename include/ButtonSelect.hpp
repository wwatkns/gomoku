#ifndef BUTTONSELECT_HPP
# define BUTTONSELECT_HPP

# include <iostream>
# include <string>
# include <vector>
# include <Eigen/Dense>
# include <SDL.h>
# include "Button.hpp"

class ButtonSelect {

public:
    ButtonSelect(std::vector<Button*> buttons);
    ButtonSelect(std::vector<Button*> buttons, Eigen::Array2i pos, int32_t padding = 5, char alignment = 'h', bool same_width = true, bool center_pos = false);
    ButtonSelect(ButtonSelect const &src);
    ~ButtonSelect(void);
    ButtonSelect	&operator=(ButtonSelect const &rhs);

    bool        get_activated_button(void) const { return _activated_button; };
    void        update_state(Eigen::Array2i *pos, bool mouse_press);
    void        render(SDL_Renderer *renderer, Eigen::Array2i *pos);

private:
    std::vector<Button*>    _buttons;
    uint32_t                _activated_button;

};

#endif
