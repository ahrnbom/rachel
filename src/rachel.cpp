#include "rachel.hpp"

namespace rachel
{
    bool shutdown = false;

    Time current_time()
    {
        return std::chrono::steady_clock::now();
    }

    void Node::main_loop()
    {
        init();
        while (!shutdown)
        {
            const auto start_time = current_time();
            handle_callbacks();
            run();

            const auto end_time = current_time();
            const auto elapsed = start_time - end_time;

            if (elapsed < time_delta)
            {
                std::this_thread::sleep_for(time_delta - elapsed);
            }
        }
    }
}