#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <utility>

namespace vanadium::lib::concurrency {

// Simple mutex-protected concurrent queue,
// which should be more than enough for our usecase

template <typename T>
class ConcurrentQueue {
 public:
  ConcurrentQueue() = default;

  ConcurrentQueue(const ConcurrentQueue&) = delete;
  ConcurrentQueue(ConcurrentQueue&&) = delete;
  ConcurrentQueue& operator=(const ConcurrentQueue&) = delete;
  ConcurrentQueue& operator=(ConcurrentQueue&&) = delete;

  void push(T value) {
    {
      std::lock_guard lock(mtx_);
      queue_.push(std::move(value));
    }
    cv_.notify_one();
  }

  bool try_pop(T& result) {
    std::lock_guard lock(mtx_);
    if (queue_.empty()) {
      return false;
    }

    result = std::move(queue_.front());
    queue_.pop();
    return true;
  }

  T pop() {
    std::unique_lock lock(mtx_);
    cv_.wait(lock, [this] {
      return !queue_.empty();
    });

    T value = std::move(queue_.front());
    queue_.pop();
    return value;
  }

  [[nodiscard]] std::size_t size() const {
    std::lock_guard lock(mtx_);
    return queue_.size();
  }

  [[nodiscard]] bool empty() const {
    std::lock_guard lock(mtx_);
    return queue_.empty();
  }

 private:
  mutable std::mutex mtx_;
  std::condition_variable cv_;
  std::queue<T> queue_;
};

}  // namespace vanadium::lib::concurrency
