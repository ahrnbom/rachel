#include "rachel.hpp"

namespace rachel {
    bool shutdown = false;
    
    Time current_time() {
        return std::chrono::steady_clock::now();
    }
}