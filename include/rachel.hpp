#pragma once
#include <chrono>
#include <thread>

namespace rachel {

using Time = std::chrono::time_point<std::chrono::steady_clock>;
using TimeDelta = Time::duration;

constexpr TimeDelta seconds(double s) {
    return std::chrono::duration_cast<TimeDelta>(std::chrono::duration<double>(s));
}

constexpr TimeDelta milliseconds(double ms) {
    return seconds(ms / 1000.0);
}

constexpr TimeDelta minutes(double m) {
    return seconds(m * 60.0);
}

constexpr TimeDelta hours(double h) { return seconds(h * 3600.0); }
constexpr TimeDelta microseconds(double us) { return seconds(us / 1000000.0); }

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
            const auto start_time = current_time();
            handle_callbacks();
            run();

            const auto end_time = current_time();
            const auto elapsed = start_time - end_time;

            if (elapsed < time_delta) {
                std::this_thread::sleep_for(time_delta - elapsed);
            }
        }
    };
};

}