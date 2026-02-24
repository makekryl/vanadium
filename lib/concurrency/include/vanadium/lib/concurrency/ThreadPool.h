#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

namespace vanadium::lib::concurrency {

class ThreadPool {
 public:
  explicit ThreadPool(std::size_t n);

  ThreadPool(const ThreadPool&) = delete;
  ThreadPool(ThreadPool&&) = delete;
  ThreadPool& operator=(const ThreadPool&) = delete;
  ThreadPool& operator=(ThreadPool&&) = delete;

  ~ThreadPool();

  void Terminate();

  [[nodiscard]] std::size_t Concurrency() const noexcept {
    return workers_.size();
  }

  void Submit(std::function<void()> f) {
    {
      std::lock_guard lock(mtx_);
      tasks_.push(std::move(f));
    }
    cv_.notify_one();
  }

  static std::size_t CurrentThreadIndex();

 private:
  std::vector<std::thread> workers_;
  std::queue<std::function<void()>> tasks_;

  std::mutex mtx_;
  std::condition_variable cv_;
  bool stop_{false};
};

}  // namespace vanadium::lib::concurrency
