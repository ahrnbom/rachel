#include <transforms_node.hpp>

#include <unordered_set>
#include <eigen3/Eigen/Geometry>

namespace transforms_node {
void canonical_frame_order(std::string& frame1, std::string& frame2) {
    if (frame2 < frame1) {
        std::swap(frame1, frame2);
    }
}

/*
    Both hash and comparison are made in such a way that A->B and B->A are "equal".
    That means that we will never store both at the same time. When looking for transforms,
    we must explicitly check the direction by checking source and target
*/
struct TransformKey {
    std::string frame1, frame2;

    bool operator==(const TransformKey& t) const noexcept {
        std::string A = t.frame1, B = t.frame2, C = frame1, D = frame2;
        canonical_frame_order(A, B);
        canonical_frame_order(C, D);

        return (A==C) && (B==D);
    }
};
}

template<>
struct std::hash<transforms_node::TransformKey>
{
    std::size_t operator()(const transforms_node::TransformKey& t) const noexcept
    {
        std::string A = t.frame1, B = t.frame2;
        transforms_node::canonical_frame_order(A, B);

        std::size_t h1 = std::hash<std::string>{}(A);
        std::size_t h2 = std::hash<std::string>{}(B);
        return h1 ^ (h2 << 1);
    }
};

namespace transforms_node {

std::unordered_map<TransformKey, Eigen::Isometry3d> transforms;

void TransformsNode::run(const nlohmann::json& params) { 
    TransformKey k;
    k.frame1 = "base";
    k.frame2 = "arm";
    transforms[k] = Eigen::Isometry3d::Identity();

    TransformKey kk;
    kk.frame1 = "arm";
    kk.frame2 = "base";
    auto it = transforms.find(kk);
    if (it != transforms.end()) {
        spdlog::info("FOUND IT!");
    } else {
        spdlog::warn("SOMETHING IS BONKERS");
    }
}

TransformsNode transforms_node("transforms_node");
}