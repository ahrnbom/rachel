#include "some_node.hpp"

SomeNode some_node("some_node");

void SomeNode::run()
{
    const auto num_pub = rachel::topics::register_publisher<int>("some_number");

    float f = NAN;
    subscribe<float>("other_number", &f);
    spdlog::info("started some node");

    int x = 0;
    while (main_loop_condition()) {

        spdlog::info("some node has f = {}", f);

        ++x;
        num_pub->publish(x);
    }
    spdlog::info("shut down some node");
}
