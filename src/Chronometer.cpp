#include "Chronometer.hpp"
#include "iostream"

Chronometer::Chronometer(void) : _running(false) {
    this->_initial_timepoint = std::chrono::steady_clock::now();
    this->_last_timepoint = this->_initial_timepoint;
    this->_elapsed = std::chrono::steady_clock::duration::zero();
}

Chronometer::Chronometer(Chronometer const &src) {
    *this = src;
}

Chronometer::~Chronometer(void) {
}

Chronometer	&Chronometer::operator=(Chronometer const &src) {
    return (*this);
}

void    Chronometer::start(void) {
    this->_initial_timepoint = std::chrono::steady_clock::now();
    this->_last_timepoint = this->_initial_timepoint;
    this->_running = true;
}

void    Chronometer::stop(void) {
    if (this->_running == true) {
        this->_elapsed += (std::chrono::steady_clock::now() - this->_last_timepoint);
        this->_running = false;
    }
}

void    Chronometer::resume(void) {
    if (this->_running == false) {
        this->_last_timepoint = std::chrono::steady_clock::now();
        this->_running = true;
    }
}

void    Chronometer::reset(void) {
    this->_running = false;
    this->_initial_timepoint = std::chrono::steady_clock::now();
    this->_last_timepoint = this->_initial_timepoint;
    this->_elapsed = std::chrono::steady_clock::duration::zero();
}

std::chrono::duration<double, std::milli>   Chronometer::get_elapsed(void) {
    if (this->_running == false)
        return this->_elapsed;
    return this->_elapsed + (std::chrono::steady_clock::now() - this->_last_timepoint);
}

uint32_t    Chronometer::get_elapsed_ms(void) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(this->get_elapsed()).count();
}
