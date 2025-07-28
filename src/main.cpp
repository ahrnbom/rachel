#include "some_node.hpp"

int main() {
    rachel::capture_interrupt_signal();
    
    SomeNode node;
    node.run();
    return 0;
}