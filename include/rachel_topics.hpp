#pragma once
#include <mutex>
#include <unordered_map>
#include <any>
#include <string>
#include <memory>



namespace rachel {
namespace topics {


template <typename T>
class Topic {
private:
    T data;
    uint64_t seq = 0;
    std::mutex mutex;

public:
    Topic(const T& t): data(t) {};
    Topic() {};

    /*
        Updates the internal state of the topic to a new piece of data, which will propagate to subscribers
    */
    void publish(const T& t) {
        const MutexLock lock(mutex);
        data = t;
        ++seq;
    }

    /*
        A subscriber can call this to keep their data up to date with the latest published data
    */
    void update(T& t, uint64_t& s) {
        const MutexLock lock(mutex);
        if (seq > s) {
            s = seq;
            t = data;
        }
    }
};

template <typename T>
using Publisher = std::shared_ptr<Topic<T>>;

inline std::unordered_map<std::string, std::any> topics;
inline std::mutex topics_mutex;

template <typename T>
Publisher<T> register_publisher(const std::string topic) {
    const MutexLock lock(topics_mutex);
    auto p = std::make_shared<Topic<T>>();
    topics[topic] = p;
    return p;
}

template <typename T>
Publisher<T> register_publisher(const std::string topic, const T& initial) {
    const MutexLock lock(topics_mutex);
    auto p = std::make_shared<Topic<T>>(initial);
    topics[topic] = p;
    return p;
}

}
}