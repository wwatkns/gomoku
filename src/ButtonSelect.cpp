#include "ButtonSelect.hpp"

ButtonSelect::ButtonSelect(std::vector<Button*> buttons) : _buttons(buttons), _activated_button(0) {
    if (this->_buttons.size() > 0)
        this->_buttons[0]->set_state(true);
}

ButtonSelect::ButtonSelect(std::vector<Button*> buttons, Eigen::Array2i pos, int32_t padding, char alignment, bool adapt_width, bool center_pos) : _buttons(buttons), _activated_button(0) {
    if (this->_buttons.size() > 0)
        this->_buttons[0]->set_state(true);

    uint32_t    max = 0;
    if (adapt_width == true) {
        for (uint32_t i = 0; i < this->_buttons.size(); i++) {
            if (this->_buttons[i]->get_rect().w > max)
                max = this->_buttons[i]->get_rect().w;
        }
        for (uint32_t i = 0; i < this->_buttons.size(); i++) {
            this->_buttons[i]->set_padding(max-this->_buttons[i]->get_rect().w+this->_buttons[i]->get_padding()[0], this->_buttons[i]->get_padding()[1]);
        }
    }
    Eigen::Array2i  tmp = pos;
    if (center_pos == true)
        tmp[0] -= (alignment == 'h' ? (int32_t)(max * (this->_buttons.size() / 2.)) : max / 2);
    for (uint32_t i = 0; i < this->_buttons.size(); i++) {
        this->_buttons[i]->set_pos(tmp);
        tmp[0] += (alignment == 'h' ? this->_buttons[i]->get_rect().w + padding : 0);
        tmp[1] += (alignment == 'v' ? this->_buttons[i]->get_rect().h + padding : 0);
    }
}

ButtonSelect::ButtonSelect(ButtonSelect const &src) {
    *this = src;
}

ButtonSelect::~ButtonSelect(void) {
}

ButtonSelect	&ButtonSelect::operator=(ButtonSelect const &src) {
    return (*this);
}

void    ButtonSelect::update_state(Eigen::Array2i *pos, bool mouse_press) {
    bool    mod = false;

    // std::cout << this->_buttons[0]->get_state() << " " << this->_buttons[1]->get_state() << std::endl;

    for (uint32_t i = 0; i < this->_buttons.size(); i++) {
        this->_buttons[i]->update_state(pos, mouse_press);
        if (this->_buttons[i]->get_state() == true) {
            this->_activated_button = i;
            mod = true;
            break;
        }
    }
    /* if after state updating all buttons are false, set last saved button to true */
    if (mod == false)
        this->_buttons[this->_activated_button]->set_state(true);

    /* put all other button states to false */
    for (uint32_t i = 0; i < this->_buttons.size(); i++) {
        if (i != this->_activated_button)
            this->_buttons[i]->set_state(false);
    }
}

void    ButtonSelect::render(SDL_Renderer *renderer, Eigen::Array2i *pos) {
    for (uint32_t i = 0; i < this->_buttons.size(); i++) {
        this->_buttons[i]->render(renderer, pos);
    }
}
