#include <transforms_node.hpp>

#include <eigen3/Eigen/Geometry>
#include <unordered_set>

#include <rachel_topics.hpp>
#include <rachel_utils.hpp>

namespace transforms_node {
void canonical_frame_order(std::string& frame1, std::string& frame2)
{
    if (frame2 < frame1) {
        std::swap(frame1, frame2);
    }
}

inline size_t two_strings_hash(const std::string& a, const std::string& b)
{
    const std::hash<std::string> x;
    const size_t h1 = x(a);
    const size_t h2 = x(b);
    return h1 ^ (h2 << 1);
}

/*
    Both hash and comparison are made in such a way that A->B and B->A are "equal".
    That means that we will never store both at the same time. When looking for transforms,
    we must explicitly check the direction by checking source and target
*/
struct TransformKey {
    std::string frame1, frame2;

    bool operator==(const TransformKey& t) const noexcept
    {
        std::string A = t.frame1, B = t.frame2, C = frame1, D = frame2;
        canonical_frame_order(A, B);
        canonical_frame_order(C, D);

        return (A == C) && (B == D);
    }
};

/*
    Represents the source -> target relationship. Unlike TransformKey, this only goes one way,
    i.e. A -> B and B -> A are considered different.
*/
struct OrderedTransformKey {
    std::string source, target;

    bool operator==(const OrderedTransformKey& t) const noexcept
    {
        return (source == t.source) && (target == t.target);
    }
};
}

template <>
struct std::hash<transforms_node::TransformKey> {
    std::size_t operator()(const transforms_node::TransformKey& t) const noexcept
    {
        std::string A = t.frame1, B = t.frame2;
        transforms_node::canonical_frame_order(A, B);

        return transforms_node::two_strings_hash(A, B);
    }
};

template <>
struct std::hash<transforms_node::OrderedTransformKey> {
    std::size_t operator()(const transforms_node::OrderedTransformKey& t) const noexcept
    {
        return transforms_node::two_strings_hash(t.source, t.target);
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
    rachel::Time stamp;

public:
    /*
        Dummy constructor needed by `std::unordered_map`
    */
    TransformObj()
    {
        source = target = "";
        transform.setIdentity();
    }

    /*
        Main constructor
    */
    TransformObj(const std::string& source, const std::string& target, const Isometry& transform, const rachel::Time& stamp)
        : source(source)
        , target(target)
        , transform(transform)
        , stamp(stamp)
    {
    }

    const std::string& get_source() const
    {
        return source;
    }

    const std::string& get_target() const
    {
        return target;
    }

    const Isometry& get_transform() const
    {
        return transform;
    }

    const rachel::Time& get_stamp() const
    {
        return stamp;
    }

    /*
        Directly multiplying the transform with a vector only works on transforming points.
        When transforming vectors that represents directions, use this function instead.
        It's implemented using homogeneous coordinates.

        TODO: Should this be placed in some kind of utility header? Should other nodes get access to this class definition?
    */
    const Eigen::Vector3d apply_to_direction(const Eigen::Vector3d& src_vec)
    {
        const Eigen::Vector4d src_vec_hom(src_vec.x(), src_vec.y(), src_vec.z(), 0);
        const Eigen::Vector4d trgt_vec_hom = transform.matrix() * src_vec_hom;
        return Eigen::Vector3d(trgt_vec_hom.x(), trgt_vec_hom.y(), trgt_vec_hom.z());
    }
};

std::unordered_map<TransformKey, TransformObj> transforms;
std::unordered_map<std::string, std::vector<std::string>> neighbors;
std::unordered_map<TransformKey, std::vector<std::string>> paths_cache;
std::unordered_map<std::string, rachel::topics::topic_ptr<Isometry>> publishers;

void add_transform(const std::string& source, const std::string& target, const Isometry& transform, const rachel::Time& stamp)
{
    // This check could be skipped in a release build
    if (source == target) {
        throw std::runtime_error("Cannot add transform from and to the same frame");
    }

    TransformKey k;
    k.frame1 = source;
    k.frame2 = target;
    transforms[k] = TransformObj(source, target, transform, stamp);

    // Add to neighbors map
    auto it = neighbors.try_emplace(source).first;
    it->second.push_back(target);
    it = neighbors.try_emplace(target).first;
    it->second.push_back(source);
}

bool _find_transform_path(const std::string& source, const std::string& target, std::vector<std::string>& visited_order, std::unordered_set<std::string> all_visited)
{
    all_visited.insert(source);
    if (source == target) {
        return true;
    }

    auto it = neighbors.find(source);
    if (it == neighbors.end()) {
        return false;
    }

    std::vector<std::string> to_visit;
    for (const std::string& frame : it->second) {
        // If we haven't visited this one yet, we should visit it
        if (all_visited.find(frame) == all_visited.end()) {
            to_visit.push_back(frame);
        }
    }

    for (const std::string& frame : to_visit) {
        if (_find_transform_path(frame, target, visited_order, all_visited)) {
            visited_order.push_back(frame);
            return true;
        }
    }

    return false;
}

/*
    Finds the path between two frames, returning true if a path can be found.
    The out vector contains all visisted frames, not including the source, including the target.
*/
bool find_transform_path(const std::string& source, const std::string& target, std::vector<std::string>& out)
{
    out.clear();

    TransformKey key;
    key.frame1 = source;
    key.frame2 = target;
    const auto it = paths_cache.find(key);
    if (it != paths_cache.end()) {
        // Already in the cache, now we just need to figure out the order
        out = it->second;
        if (out.back() != target) {
            std::reverse(out.begin(), out.end());
        }
        return true;
    }

    std::unordered_set<std::string> all_visited;
    if (_find_transform_path(source, target, out, all_visited)) {
        // the order is backwards because of recursive traversal
        std::reverse(out.begin(), out.end());

        // store in cache
        paths_cache[key] = out;

        return true;
    }
    return false;
}

bool find_transform(const std::string& source, const std::string& target, Isometry& out)
{
    std::vector<std::string> order;
    bool success = find_transform_path(source, target, order);

    out.setIdentity();
    TransformKey key;
    key.frame1 = source;
    for (const std::string& frame : order) {
        key.frame2 = frame;
        auto it = transforms.find(key);

        // This check could be skipped in a release build
        if (it == transforms.end()) {
            spdlog::error("Transform tree broken, found a path that goes through missing transforms");
            return false;
        }

        Isometry T = it->second.get_transform();
        if (it->second.get_source() == frame) {
            // Opposite order, invert
            T = T.inverse();
        }
        out = T * out;

        key.frame1 = frame;
    }

    return success;
}

bool interpret_topic_name(const std::string& topic, std::string& source, std::string& target)
{
    const auto pos = topic.find("->");
    if (pos == std::string::npos || pos == 0 || pos >= topic.size() - 3) {
        return false;
    }

    source = topic.substr(0, pos);
    target = topic.substr(pos + 2);
    return true;
}

void TransformsNode::run(const nlohmann::json& params)
{
    while (main_loop_condition()) {
        // Any new requested transforms?
        std::unordered_set<std::string> topics;
        rachel::topics::find_topics_by_tag("transform-sub", topics);
        for (const std::string& topic : topics) {
            const auto found = publishers.find(topic);
            if (found == publishers.end()) {
                // No publisher yet, let's create it
                publishers[topic] = rachel::topics::register_publisher<Isometry>(topic);
            }
        }

        // For each publisher, find requested transform and publish it
        for (const auto& pair : publishers) {
            const std::string& topic = pair.first;
            const auto& pub = pair.second;

            std::string source, target;
            if (!interpret_topic_name(topic, source, target)) {
                spdlog::warn("transforms_node: invalid topic tagged as 'transform-sub': {}", topic);
                continue;
            }

            Isometry iso;
            if (find_transform(source, target, iso)) {
                pub->publish(iso);
            }
        }
    }
}

TransformsNode transforms_node("transforms_node");
}