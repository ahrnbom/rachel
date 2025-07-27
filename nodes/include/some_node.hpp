#pragma once
#include "rachel.hpp"
#include <iostream>

class SomeNode : public rachel::Node {
public:
    rachel::Publisher<int> num_pub;
    float f;

    void init() override;

    void run() override; 
};
