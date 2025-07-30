#include "other_node.hpp"

int x = 0;

void OtherNode::run()
{
    auto pub = rachel::topics::register_publisher<float>("other_number");

    subscribe<int>("some_number", [&](const int& i) {
        this->do_something(i);
    });
    spdlog::info("started other node");

    while (main_loop_condition()) {
        pub->publish(5 + 0.1f*x);
    }

    spdlog::info("shut down other node");
}

void OtherNode::do_something(const int& i) {
    spdlog::info("other node got {}", i);
    x = i;
}
