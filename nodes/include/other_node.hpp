#pragma once
#include "rachel.hpp"

class OtherNode : public rachel::Node {
private:
    int x = 0;
    
public:
    using Node::Node;
    void run(const nlohmann::json& params) override; 
    void do_something(const int& i);
};

extern OtherNode other_node;