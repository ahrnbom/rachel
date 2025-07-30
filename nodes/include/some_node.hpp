#pragma once
#include "rachel.hpp"

class SomeNode : public rachel::Node {
public:
    using Node::Node;
    void run() override; 
};

extern SomeNode some_node;