#include "some_node.hpp"

void SomeNode::init()
{
    std::cout << "Initializing node..." << std::endl;
    num_pub = rachel::topics::register_publisher<int>("some_number");
    std::cout << "Created a publisher!" << std::endl;
    subscribe<float>("other_number", &f);
    std::cout << "Created a subscriber!" << std::endl;
}

void SomeNode::run()
{
    std::cout << "Running node..." << std::endl;
    num_pub->publish(5);

    // Add some delay or exit condition
    rachel::shutdown = true;
}
