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
        using seq_t = size_t;

        template <typename T>
        class Topic
        {
        private:
            std::deque<T> queue;
            size_t queue_size = 4;
            seq_t newest_seq_in_queue = 0;
            std::mutex mutex;

        public:
            Topic() {
                newest_seq_in_queue = 0;
            };

            /*
                Updates the internal state of the topic to a new piece of data, which will propagate to subscribers
            */
            void publish(const T &t)
            {
                const MutexLock lock(mutex);
                queue.push_back(t);
                ++newest_seq_in_queue;

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

                if (newest_seq_in_queue > seq_num) {
                    seq_num = newest_seq_in_queue;
                    t = queue.back();
                    is_set = true;
                }
            }

            /*
                A subscriber can call this to perform a callback function on each value with a newer
                sequence number than what they had stored. This also updates said sequence number. 
            */
            void perform_callbacks(seq_t& s, std::function<void(const T&)>& cb) {
                const MutexLock lock(mutex);

                const size_t qs = queue.size();
                if (s >= newest_seq_in_queue || qs == 0 || newest_seq_in_queue == 0) {
                    return;
                }

                /*
                    We want to loop over the last N elements in the queue, where N is the number of elements we haven't seen yet.
                    We must also make sure all of those exist in the queue, it is okay that we may skip elements that have already 
                    left the queue
                */

                const size_t N = qs - std::min(newest_seq_in_queue - s, qs);
                for (auto it = queue.begin() + N; it != queue.end(); ++it) {
                    cb(*it);
                }
                s = newest_seq_in_queue;
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
        topic_ptr<T> _register_publisher_raw(const std::string topic)
        {
            topic_ptr<T> p;

            auto found = topics.find(topic);
            if (found == topics.end())
            {
                p = std::make_shared<Topic<T>>();
                topics[topic] = p;
            }
            else
            {
                p = std::any_cast<topic_ptr<T>>(found->second);
            }

            return p;
        }

        /*
            Register a publisher for a new topic.
        */
        template <typename T>
        topic_ptr<T> register_publisher(const std::string topic)
        {
            const MutexLock lock(topics_mutex);
            return _register_publisher_raw<T>(topic);
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
                return _register_publisher_raw<T>(topic);
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