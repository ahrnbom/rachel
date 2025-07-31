#include "some_node.hpp"

SomeNode some_node("some_node");

const std::string INITIAL_NUMBER_PARAM = "/some_node/initial_number";

void SomeNode::set_default_params(nlohmann::json& params) {
    params[INITIAL_NUMBER_PARAM] = 0;
}

void SomeNode::run()
{
    const auto num_pub = rachel::topics::register_publisher<int>("some_number");

    const auto& params = *rachel::params;

    float f;
    bool f_set = false;
    subscribe<float>("other_number", &f, &f_set);
    spdlog::info("started some node");

    int x = params[INITIAL_NUMBER_PARAM];
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
