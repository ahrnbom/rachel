#include "other_node.hpp"

void OtherNode::run()
{
    auto pub = rachel::topics::register_publisher<float>("other_number");

    subscribe<int>("some_number", [&](const int& i) {
        this->do_something(i);
    });
    std::cout << "other node is initialized!" << std::endl;

    while (main_loop_condition()) {
        std::cout << "hello from other node!" << std::endl;
        pub->publish(5);
    }

    std::cout << "shut down other node!" << std::endl;
}

void OtherNode::do_something(const int& i) {
    std::stringstream ss;
    ss << "other node got the value " << i;
    std::cout << ss.str() << std::endl;
}
