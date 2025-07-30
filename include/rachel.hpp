#pragma once
#include <chrono>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <any>
#include <functional>
#include <iostream>
#include <atomic>
#include <cmath>

using MutexLock = std::lock_guard<std::mutex>;

#include "rachel_topics.hpp"

namespace rachel
{
    using Time = std::chrono::time_point<std::chrono::steady_clock>;
    using TimeDelta = Time::duration;

    constexpr TimeDelta seconds(double s)
    {
        return std::chrono::duration_cast<TimeDelta>(std::chrono::duration<double>(s));
    }

    constexpr TimeDelta microseconds(double us) { return seconds(us / 1000000.0); }
    constexpr TimeDelta milliseconds(double ms) { return seconds(ms / 1000.0); }
    constexpr TimeDelta minutes(double m) { return seconds(m * 60.0); }
    constexpr TimeDelta hours(double h) { return seconds(h * 3600.0); }

    /*
        Obtains the current time, taking into account if the system is running live or from a recording.
    */
    Time current_time();

    /*
        Set to true when it is time to shut down the program. All nodes are expected to quickly turn themselves off,
        which can be achieved by regularly using main_loop_condition.
        It is marked as `extern` so that all nodes share the same object, and it is atomic for thread safety.
        The actual value is defined in rachel.cpp
    */
    extern std::atomic<bool> shutdown;

    void capture_interrupt_signal();

    class Node
    {
    private:
        std::unordered_map<std::string, std::function<void(void)>> _subscription_updates;
        std::unordered_map<std::string, std::any> _subscriptions;
        Time _last_loop_condition;
        TimeDelta _time_delta = seconds(0.1);

    public:
        Node();

        template <typename T>
        void subscribe(const std::string &topic, T *data)
        {
            _subscriptions[topic] = topics::ValueSubscription<T>(data, topic);
            _subscription_updates[topic] = [this, topic]()
            {
                auto sub = std::any_cast<topics::ValueSubscription<T>>(_subscriptions[topic]);
                sub.update();
            };
        };

        void set_time_delta(const TimeDelta &dt);
        virtual void handle_callbacks();
        virtual void run() {};
        virtual bool main_loop_condition();
    };

    /*
        Launches a node in its own thread
    */
    void launch(Node& node);

    /*
        Waits for all nodes to finish. Typically called near the end of the main function.
    */
    void wait_for_nodes();
}