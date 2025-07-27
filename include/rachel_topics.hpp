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

    /*
        Overrides the sequence number. Calling this from a custom node will probably break the subscription.
        This is used for initialization, to determine if the initial value should be provided to subscribers
        or not.  
    */
    void _override_seq(const uint64_t& new_seq) {
        seq = new_seq;
    }
};

template <typename T>
using topic_ptr = std::shared_ptr<Topic<T>>;

inline std::unordered_map<std::string, std::any> topics;
inline std::mutex topics_mutex;

/*
    This function registers a publisher without obtaining the topics lock, 
    so it should only be called in contexts where the lock has already been obtained
*/
template <typename T>
topic_ptr<T> _register_publisher_raw(const std::string topic, const T& initial, const uint64_t& seq) {
    topic_ptr<T> p;

    auto found = topics.find(topic);
    if (found == topics.end()) {
        p = std::make_shared<Topic<T>>(initial);
        p->_override_seq(seq);
        topics[topic] = p;
    } else {
        p = std::any_cast<topic_ptr<T>>(found->second);
        p->publish(initial);
    }

    return p;
}

/*
    Registers a publisher for a new topic, with some initial value provided.
    This value will be provided to any subscriber until some other value is published
    on this topic.
*/
template <typename T>
topic_ptr<T> register_publisher(const std::string topic, const T& initial) {
    const MutexLock lock(topics_mutex);
    return _register_publisher_raw(topic, initial, 1);
}

/*
    Register a publisher for a new topic. Subscribers will not get any data until 
    something has been published on this topic.
*/
template <typename T>
topic_ptr<T> register_publisher(const std::string topic) {
    T initial;
    const MutexLock lock(topics_mutex);
    return _register_publisher_raw(topic, initial, 0);
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
            T initial;
            p = _register_publisher_raw<T>(topic, initial, 0);
        }
    };

    void update() {
        p->update(*t, seq);
    }
};

}
}