#pragma once
#include "rachel.hpp"

class SomeNode : public rachel::Node {
public:
    using Node::Node;
    void run() override; 
    void set_default_params(nlohmann::json& params) override;
};

extern SomeNode some_node;