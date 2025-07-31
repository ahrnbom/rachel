#include "rachel.hpp"
#include <signal.h>

namespace rachel
{
    std::atomic<bool> shutdown{false};

    Time current_time()
    {
        return std::chrono::steady_clock::now();
    }

    Node::Node(const std::string& name): node_name(name) {
        _last_loop_condition = current_time();
    }

    void Node::handle_callbacks()
    {
        for (const auto &sub : _subscription_updates)
        {
            sub.second();
        }
    }

    void Node::set_time_delta(const TimeDelta &dt)
    {
        _time_delta = dt;
    }

    bool Node::main_loop_condition()
    {
        if (shutdown)
        {
            return false;
        }

        handle_callbacks();

        const auto elapsed = current_time() - _last_loop_condition;
        const auto remaining = _time_delta - elapsed;
        
        if (remaining > TimeDelta::zero())
        {
            std::this_thread::sleep_for(remaining);
            spdlog::debug("{} took {:.4f} s out of {:.4f} s budget", node_name, to_seconds(elapsed), to_seconds(_time_delta));
        } else {
            spdlog::warn("{} took {:.4f} s out of {:.4f} s budget", node_name, to_seconds(elapsed), to_seconds(_time_delta));
        }
        _last_loop_condition = current_time();
        return true;
    }

    void Node::set_default_params(nlohmann::json& params) {

    }

    void capture_interrupt_signal()
    {
        struct sigaction sa;
        sa.sa_handler = [](int s)
        { shutdown = true; };
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGINT, &sa, NULL);
    }

    std::vector<Node*> launched_nodes;
    void launch(Node& node) {
        launched_nodes.push_back(&node);
    }

    const nlohmann::json* params = NULL;
    void start() {
        // Load all default parameters
        Parameters _params;
        for (Node* node : launched_nodes)  {
            _params.load_default_params([node](nlohmann::json& data) {
                node->set_default_params(data);
            });
        }

        // Load parameters from file
        _params.load_from_file();

        // Update params pointer for nodes to use
        _params.finalize();
        params = _params.get();

        // Start the threads
        std::vector<std::thread> threads;
        for (Node* node : launched_nodes) {
            spdlog::info("Starting node: {}", node->node_name);
            threads.push_back(std::thread([node](){node->run();}));
        }
        
        // Wait for them to finish
        for (auto& t: threads) {
            t.join();
        }
    }
}