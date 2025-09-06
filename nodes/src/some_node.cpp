#include "some_node.hpp"

#include <eigen3/Eigen/Geometry>

SomeNode some_node("some_node");

const std::string INITIAL_NUMBER_PARAM = some_node.param_name("~/initial_number");

using Isometry = Eigen::Isometry3d;

void SomeNode::set_default_params(nlohmann::json& params)
{
    params[INITIAL_NUMBER_PARAM] = 0;
}

void SomeNode::run(const nlohmann::json& params)
{
    const auto num_pub = rachel::topics::register_publisher<int>("some_number");
    const auto tf1_pub = rachel::topics::register_publisher<Isometry>("A->B");
    const auto tf2_pub = rachel::topics::register_publisher<Isometry>("B->C");
    const auto tf3_pub = rachel::topics::register_publisher<Isometry>("A->D");

    float f;
    bool f_set = false;
    subscribe<float>("other_number", f, f_set);
    spdlog::info("started some node");

    int x = params[INITIAL_NUMBER_PARAM];
    double angle = 0.0;
    while (main_loop_condition()) {
        if (!f_set) {
            spdlog::warn("some node has no f value yet!");
        } else {
            spdlog::info("some node has f = {}", f);
        }

        ++x;
        num_pub->publish(x);

        Isometry transform;
        transform.setIdentity();
        transform.translate(Eigen::Vector3d(0.5, 0.5, 1.0));
        tf1_pub->publish(transform);

        angle += 0.01;
        transform.rotate(Eigen::AngleAxisd(angle, Eigen::Vector3d::UnitX()));
        tf2_pub->publish(transform);

        transform.translate(Eigen::Vector3d(0.1, 0.2, 0.3));
        tf3_pub->publish(transform);
    }
    spdlog::info("shut down some node");
}
