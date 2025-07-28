#include "some_node.hpp"
#include "other_node.hpp"

int main() {
    rachel::capture_interrupt_signal();

    SomeNode some_node;
    rachel::launch(some_node);

    OtherNode other_node;
    rachel::launch(other_node);

    rachel::wait_for_nodes();

    std::cout << "le done" << std::endl;
    return 0;
}