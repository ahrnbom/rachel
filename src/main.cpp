#include "rachel.hpp"
#include <iostream>

class MyNode : public rachel::Node {
public:
    void run() override {
        std::cout << "Running node..." << std::endl;
        // Add some delay or exit condition
        rachel::shutdown = true;
    }
};

int main() {
    MyNode node;
    node.main_loop();
    return 0;
}