#include "other_node.hpp"


void OtherNode::run()
{
    int f = -1;

    auto pub = rachel::topics::register_publisher<float>("other_number");

    subscribe<int>("some_number", &f);
    std::cout << "hello from other node!" << std::endl;

    while (main_loop_condition()) {
        std::cout << "hello from other node! f = " << f << std::endl;
        pub->publish(5);
    }

    std::cout << "shut down other node!" << std::endl;
}
