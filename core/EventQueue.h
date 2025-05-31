#pragma once
#include <queue>
#include <mutex>
#include <optional>
#include <condition_variable>

// Потокобезопасная очередь для любых T
template<typename T>
class EventQueue {
public:
    void push(const T& item) {
        std::lock_guard<std::mutex> lg(mutex_);
        queue_.push(item);
        cv_.notify_one();
    }

    // Блокирующий pop
    T pop() {
        std::unique_lock<std::mutex> ul(mutex_);
        cv_.wait(ul, [&] { return !queue_.empty(); });
        T item = queue_.front(); queue_.pop();
        return item;
    }

    // Неблокирующий try_pop
    bool try_pop(T& out) {
        std::lock_guard<std::mutex> lg(mutex_);
        if (queue_.empty()) return false;
        out = queue_.front(); queue_.pop();
        return true;
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable cv_;
};
