#include "some_node.hpp"


void SomeNode::run()
{
    const auto num_pub = rachel::topics::register_publisher<int>("some_number");

    float f = NAN;
    subscribe<float>("other_number", &f);
    std::cout << "hello from some node!" << std::endl;

    int x = 0;
    while (main_loop_condition()) {
        std::cout << "hello from some node! f = " << f << std::endl;

        ++x;
        num_pub->publish(x);
    }

    std::cout << "shut down some node!" << std::endl;
}
