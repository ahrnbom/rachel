#include "rachel.hpp"

namespace rachel {
    bool shutdown = false;
    
    Time current_time() {
        return std::chrono::system_clock::now();
    }
}