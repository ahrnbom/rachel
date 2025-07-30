#include "some_node.hpp"
#include "other_node.hpp"

int main() {
    rachel::capture_interrupt_signal();

    rachel::launch(some_node);
    rachel::launch(other_node);

    rachel::wait_for_nodes();

    spdlog::info("rachel done!");
    return 0;
}