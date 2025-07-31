#include "some_node.hpp"
#include "other_node.hpp"

int main() {
    rachel::capture_interrupt_signal();

    rachel::launch(some_node);
    rachel::launch(other_node);

    rachel::start();

    spdlog::info("rachel done!");
    return 0;
}