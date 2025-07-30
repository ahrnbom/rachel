#include "some_node.hpp"

SomeNode some_node("some_node");

void SomeNode::run()
{
    const auto num_pub = rachel::topics::register_publisher<int>("some_number");

    float f;
    bool f_set = false;
    subscribe<float>("other_number", &f, &f_set);
    spdlog::info("started some node");

    int x = 0;
    while (main_loop_condition())
    {
        if (!f_set)
        {
            spdlog::warn("some node has no f value yet!");
        }
        else
        {
            spdlog::info("some node has f = {}", f);
        }

        ++x;
        num_pub->publish(x);
    }
    spdlog::info("shut down some node");
}
