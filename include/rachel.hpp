#pragma once
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <any>
#include <functional>
#include <iostream>

using MutexLock = std::lock_guard<std::mutex>;

#include "rachel_topics.hpp"

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



/*
    Obtains the current time, taking into account if the system is running live or from a recording.
*/
Time current_time();

extern bool shutdown;

class Node {
private:
    std::unordered_map<std::string, std::function<void(void)>> subscription_updates;
    std::unordered_map<std::string, std::any> subscriptions;
    

public: 
    TimeDelta time_delta = seconds(0.1);

    template <typename T>
    void subscribe(const std::string& topic, T* data) {
        subscriptions[topic] = topics::Subscription<T>(data, topic);
        subscription_updates[topic] = [this, topic]() {
            auto sub = std::any_cast<topics::Subscription<T>>(subscriptions[topic]);
            sub.update();
        };
    };

    virtual void handle_callbacks() {
        for (const auto& sub : subscription_updates) {
            sub.second();
        }
    };

    virtual void init() {};
    virtual void run() {};
    virtual void main_loop();
};

template <typename T>
using Publisher = topics::topic_ptr<T>;

}