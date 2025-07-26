#include "rachel.hpp"
#include <iostream>

class SomeNode : public rachel::Node {
public:
    rachel::Publisher<int> num_pub;
    float f;

    void init() override {
        std::cout << "Initializing node..." << std::endl;
        num_pub = rachel::topics::register_publisher<int>("some_number");

        subscribe<float>("other_number", &f);
    }

    void run() override {
        std::cout << "Running node..." << std::endl;
        num_pub->publish(5);
        
        // Add some delay or exit condition
        rachel::shutdown = true;
    }
};

int main() {
    SomeNode node;
    node.main_loop();
    return 0;
}