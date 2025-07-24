#pragma once
#include <chrono>

namespace rachel {

using Time = std::chrono::time_point<std::chrono::system_clock>;
using DeltaTime = std::chrono::duration<double>;

Time current_time();

extern bool shutdown;

class Node {
public: 
    DeltaTime time_delta(0.1);

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