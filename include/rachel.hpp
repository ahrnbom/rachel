#pragma once
#include <chrono>
#include <thread>
#include <mutex>

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

using MutexLock = std::lock_guard<std::mutex>;

/*
    Obtains the current time, taking into account if the system is running live or from a recording.
*/
Time current_time();

extern bool shutdown;

template <typename T>
class Topic {
private:
    T data;
    uint64_t seq = 0;
    std::mutex mutex;

public:
    Topic(const T& t): data(t) {};
    Topic() {};

    /*
        Updates the internal state of the topic to a new piece of data, which will propagate to subscribers
    */
    void publish(const T& t);

    /*
        A subscriber can call this to keep their data up to date with the latest published data
    */
    void update(T& t, uint64_t& s);
};

class Node {
public: 
    TimeDelta time_delta = seconds(0.1);

    virtual void handle_callbacks() {};
    virtual void init() {};
    virtual void run() {};
    virtual void main_loop();
};

}