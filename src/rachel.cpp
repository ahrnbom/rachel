#include "rachel.hpp"

namespace rachel
{
    bool shutdown = false;

    Time current_time()
    {
        return std::chrono::steady_clock::now();
    }

    void Node::handle_callbacks() {
            for (const auto &sub : _subscription_updates)
            {
                sub.second();
            }
    }

    void Node::set_time_delta(const TimeDelta& dt) {
        _time_delta = dt;
    }

    bool Node::main_loop_condition()
    {
        if (shutdown) {
            return false;
        }

        std::cout << "1" << std::endl;
        handle_callbacks();

        std::cout << "2" << std::endl;
        const auto now = current_time();
        const auto elapsed = now - _last_loop_condition;
        const auto remaining = _time_delta - elapsed;
        _last_loop_condition = now;
        if (remaining > TimeDelta::zero()) {
            std::cout << "3" << std::endl;
            std::this_thread::sleep_for(remaining);
        }
        std::cout << "4" << std::endl;
        return true;
    }
}