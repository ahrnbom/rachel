#pragma once
#include "rachel.hpp"

class OtherNode : public rachel::Node {
public:
    void run() override; 
    void do_something(const int& i);
};