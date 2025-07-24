#pragma once
#include <chrono>

namespace rachel {

using Time = std::chrono::time_point<std::chrono::system_clock>;
using TimeDelta = std::chrono::duration<double>;

constexpr TimeDelta seconds(double s) {return TimeDelta(s); }
constexpr TimeDelta milliseconds(double ms) { return TimeDelta(ms / 1000.0); }
constexpr TimeDelta minutes(double m) { return TimeDelta(m * 60.0); }

Time current_time();

extern bool shutdown;

class Node {
public: 
    TimeDelta time_delta = seconds(0.1);

    virtual void handle_callbacks() {};
    virtual void init() {};
    virtual void run() {};
    virtual void main_loop() {
        init();
        while (!shutdown) {
            handle_callbacks();
            run();
        }
    };
};

}