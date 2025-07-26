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
    uint64_t seq = 1;
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
using topic_ptr = std::shared_ptr<Topic<T>>;

inline std::unordered_map<std::string, std::any> topics;
inline std::mutex topics_mutex;

template <typename T>
topic_ptr<T> register_publisher(const std::string topic, const T& initial) {
    const MutexLock lock(topics_mutex);

    topic_ptr<T> p;

    auto found = topics.find(topic);
    if (found == topics.end()) {
        p = std::make_shared<Topic<T>>(initial);
        topics[topic] = p;
    } else {
        p = std::any_cast<topic_ptr<T>>(found->second);
        p->publish(initial);
    }

    return p;
}

template <typename T>
topic_ptr<T> register_publisher(const std::string topic) {
    const MutexLock lock(topics_mutex);
    T initial;
    return register_publisher(topic, initial);
}

template <typename T>
class Subscription {
private:
    T* t;
    topic_ptr<T> p;
    std::string topic_name;
    uint64_t seq = 0;

public:
    Subscription(T* data, const std::string& topic): t(data), topic_name(topic) {
        const MutexLock lock(topics_mutex);
        auto found = topics.find(topic_name);
        if (found != topics.end()) {
            p = std::any_cast<topic_ptr<T>>(found->second);
        } else {
            p = register_publisher<T>(topic);
        }
    };

    void update() {
        p->update(*t, seq);
    }
};

}
}