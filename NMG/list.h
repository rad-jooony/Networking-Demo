#pragma once

#include <algorithm>
#include <functional>
#include <list>
#include <mutex>

template <typename T>
class List
{
    public:
        void for_each(std::function<void(T)> function) //Apply a function to each in a loop
        {
            std::unique_lock<std::mutex> mlock(mutex_);
            std::for_each(list_.begin(), list_.end(), function);
        }

        void push(const T& item) // adds a new item to the end of the list
        {
            std::unique_lock<std::mutex> mlock(mutex_);
            list_.push_back(item);
        }

        unsigned int size()
        {
            return list_.size();
        }

        List() {};
        List(const List&) = delete;            // disable copying
        List& operator=(const List&) = delete; // disable assignment

    private:
        std::list<T> list_;
        std::mutex mutex_;
};
