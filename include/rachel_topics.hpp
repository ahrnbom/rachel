#pragma once
#include <mutex>
#include <unordered_map>
#include <any>
#include <string>
#include <memory>
#include <deque>
#include <sstream>

namespace rachel
{
    namespace topics
    {
        using seq_t = uint64_t;

        template <typename T>
        class Topic
        {
        private:
            std::deque<std::pair<seq_t, T>> queue;
            size_t queue_size = 4;
            seq_t seq;
            std::mutex mutex;

        public:
            Topic(const T &t, const seq_t& first_seq) : seq(first_seq) {
                publish(t);
            };
            Topic() {
                seq = 0;
            };

            /*
                Updates the internal state of the topic to a new piece of data, which will propagate to subscribers
            */
            void publish(const T &t)
            {
                const MutexLock lock(mutex);
                queue.push_back(std::make_pair(seq, t));
                ++seq;

                while (queue.size() > queue_size) {
                    queue.pop_front();
                }
            }

            /*
                A subscriber can call this to keep their data up to date with the latest published data
            */
            void update(T &t, seq_t &seq_num, bool& is_set)
            {
                const MutexLock lock(mutex);
                if (queue.size() == 0) {
                    return;
                }

                const std::pair<seq_t, T>& pair = queue.back();
                if (pair.first > seq_num) {
                    seq_num = pair.first;
                    t = pair.second;
                    is_set = true;
                }
            }

            /*
                A subscriber can call this to perform a callback function on each value with a newer
                sequence number than what they had stored. This also updates said sequence number. 
            */
            void perform_callbacks(seq_t& s, std::function<void(const T&)>& cb) {
                const MutexLock lock(mutex);

                /*
                    Possible optimization
                    This loop can be avoided in several cases, in particular when we're entirely up to date
                    It's a little bit tricky to write something completely loop free that also isn't very brittle
                */
                for (const std::pair<seq_t, T>& pair : queue) {
                    if (pair.first > s) {
                        cb(pair.second);
                        s = pair.first;
                    }
                }
            }

            /*
                Modifies the queue size. This does not directly affect any values currently in the queue.
                If the queue size is reduced, values will get removed on the next time a new value is published.
            */
            void set_queue_size(const size_t& new_queue_size) {
                const MutexLock lock(mutex);
                queue_size = new_queue_size;
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
        topic_ptr<T> _register_publisher_raw(const std::string topic, const T &initial, const seq_t &seq)
        {
            topic_ptr<T> p;

            auto found = topics.find(topic);
            if (found == topics.end())
            {
                p = std::make_shared<Topic<T>>(initial, seq);
                topics[topic] = p;
            }
            else
            {
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
        topic_ptr<T> register_publisher(const std::string topic, const T &initial)
        {
            const MutexLock lock(topics_mutex);
            return _register_publisher_raw(topic, initial, 1);
        }

        /*
            Register a publisher for a new topic. Subscribers will not get any data until
            something has been published on this topic.
        */
        template <typename T>
        topic_ptr<T> register_publisher(const std::string topic)
        {
            T initial;
            const MutexLock lock(topics_mutex);
            return _register_publisher_raw(topic, initial, 0);
        }

        template <typename T>
        topic_ptr<T> find_topic(const std::string& topic) {
            const MutexLock lock(topics_mutex);
            auto found = topics.find(topic);
            if (found != topics.end())
            {
                return std::any_cast<topic_ptr<T>>(found->second);
            }
            else
            {
                T initial;
                return _register_publisher_raw<T>(topic, initial, 0);
            }
        }

        /*
            Simple subscription that keeps a value up-to-date with the latest published value.
            This disregards the queue, only the latest published value will be written.
        */
        template <typename T>
        class ValueSubscription
        {
        private:
            T *t;
            topic_ptr<T> p;
            std::string topic_name;
            seq_t seq = 0;
            bool has_been_set = false;

        public:
            ValueSubscription(T *data, const std::string &topic) : t(data), topic_name(topic)
            {
                p = find_topic<T>(topic_name);
            };

            void update()
            {
                p->update(*t, seq, has_been_set);
            }

            bool is_set() {
                return has_been_set;
            }
        };

        template <typename T>
        class QueueSubscription {
        private:
            seq_t seq = 0;
            std::string topic_name;
            topic_ptr<T> p;
            
        public:
            QueueSubscription(const std::string& topic): topic_name(topic) {
                p = find_topic<T>(topic_name);
            }

            void update(std::function<void(const T&)> callback) {
                p->perform_callbacks(seq, callback);
            }
        };
    }
}