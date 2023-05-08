#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class Queue
{
    public:
        T pop() // Blocking pop. Wait until an item is in the queue
        {
            std::unique_lock<std::mutex> mlock(_mutex);
            _cond.wait(mlock, [this]{return !_queue.empty();});
            auto val = _queue.front();
            _queue.pop();
            return val;
        }

        void pop(T& item) // pop will not wait with an empty queue
        {
            std::unique_lock<std::mutex> mlock(_mutex);
            if (_queue.empty())
            {
                return;
            }
            item = _queue.front();
            _queue.pop();
        }

        void push(const T& item) // push an item to the back of the queue
        {
            std::unique_lock<std::mutex> mlock(_mutex);
            _queue.push(item);
            _cond.notify_one();
        }

        Queue() {};
        Queue(const Queue&) = delete;            // disable copying
        Queue& operator=(const Queue&) = delete; // disable assignment

    private:
        std::queue<T> _queue;
        std::mutex _mutex;
        std::condition_variable _cond;
};
