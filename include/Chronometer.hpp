#ifndef CHRONOMETER_HPP
# define CHRONOMETER_HPP

# include <vector>
# include <string>
# include <chrono>

class Chronometer {

public:
    Chronometer(void);
    Chronometer(Chronometer const &src);
    ~Chronometer(void);
    Chronometer	&operator=(Chronometer const &rhs);

    void                                        start(void);
    void                                        stop(void);
    void                                        resume(void);
    void                                        reset(void);

    std::chrono::duration<double, std::milli>   get_elapsed(void);
    uint32_t                                    get_elapsed_ms(void);

    bool                                        is_running(void) const { return _running; };
    std::chrono::steady_clock::time_point       get_initial_timepoint(void) const { return _initial_timepoint; };
    void                                        set_initial_timepoint(std::chrono::steady_clock::time_point timepoint) { _initial_timepoint = timepoint; };

private:
    std::chrono::steady_clock::time_point       _initial_timepoint;
    std::chrono::steady_clock::time_point       _last_timepoint;
    std::chrono::duration<double, std::milli>   _elapsed;

    bool    _running;
};

#endif
