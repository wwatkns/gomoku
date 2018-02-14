#include "ButtonSelect.hpp"

ButtonSelect::ButtonSelect(std::vector<Button*> buttons) : _buttons(buttons) {
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

    for (uint32_t i = 0; i < this->_buttons.size(); i++) {
        this->_buttons[i]->update_state(pos, mouse_press);
        if (this->_buttons[i]->get_state() == true) {
            this->_activated_button = i;
            mod = true;
            break;
        }
    }
    /* if after state updating all buttons are false, set last saved button to true */
    if (!mod)
        this->_buttons[this->_activated_button]->set_state(true);
        
    /* put all other button states to false */
    for (uint32_t i = 0; i < this->_buttons.size(); i++) {
        if (i != this->_activated_button)
            this->_buttons[i]->set_state(false);
    }
}

void    ButtonSelect::render(SDL_Renderer *renderer) {
    for (uint32_t i = 0; i < this->_buttons.size(); i++) {
        this->_buttons[i]->render(renderer);
    }
}
