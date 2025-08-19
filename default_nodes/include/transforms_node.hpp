#pragma once

#include "rachel.hpp"

namespace transforms_node {

class TransformsNode : public rachel::Node {
private:
    int x = 0;

public:
    using Node::Node;
    void run(const nlohmann::json& params) override;
};

extern TransformsNode transforms_node;
}