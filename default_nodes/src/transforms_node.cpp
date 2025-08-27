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

using Isometry = Eigen::Isometry3d;

/*
    Represents a transform T such that if X is a point in the source frame, then Y=T*X will be the corresponding point in
    the target frame. To transform a direction, use the `apply_to_direction` function.
*/
class TransformObj {
private:
    Isometry transform;
    std::string source, target;

public:
    /*
        Dummy constructor needed by `std::unordered_map`
    */
    TransformObj() {
        source = target = "";
        transform.setIdentity();
    }

    /*
        Main constructor
    */
    TransformObj(const std::string& source, const std::string& target, const Isometry& transform): source(source), target(target), transform(transform) {

    }

    const std::string& get_source() const {
        return source;
    }

    const std::string& get_target() const {
        return target;
    }

    const Isometry& get_transform() const {
        return transform;
    }

    /*
        Directly multiplying the transform with a vector only works on transforming points. 
        When transforming vectors that represents directions, use this function instead.
        It's implemented using homogeneous coordinates.

        TODO: Should this be placed in some kind of utility header? Should other nodes get access to this class definition?
    */
    const Eigen::Vector3d apply_to_direction(const Eigen::Vector3d& src_vec) {
        const Eigen::Vector4d src_vec_hom(src_vec.x(), src_vec.y(), src_vec.z(), 0);
        const Eigen::Vector4d trgt_vec_hom = transform.matrix() * src_vec_hom;
        return Eigen::Vector3d(trgt_vec_hom.x(), trgt_vec_hom.y(), trgt_vec_hom.z());
    }
};

std::unordered_map<TransformKey, TransformObj> transforms;

void add_transform(const std::string& source, const std::string& target, const Isometry& transform) {
    TransformKey k;
    k.frame1 = source;
    k.frame2 = target;
    transforms[k] = TransformObj(source, target, transform);
}

void TransformsNode::run(const nlohmann::json& params) { 
    add_transform("base", "arm", Isometry::Identity());

    TransformKey kk;
    kk.frame1 = "arm";
    kk.frame2 = "base";
    auto it = transforms.find(kk);
    if (it != transforms.end()) {
        const Isometry t = it->second.get_transform();
        std::stringstream ss;
        ss << t.matrix();
        spdlog::info("FOUND IT!\n{}", ss.str());
    } else {
        spdlog::warn("SOMETHING IS BONKERS");
    }
}

TransformsNode transforms_node("transforms_node");
}