#include "vanadium/lib/concurrency/ThreadPool.h"

#include <thread>

namespace vanadium::lib::concurrency {

namespace {
thread_local std::size_t tl_pooled_thread_idx{0};
}

ThreadPool::ThreadPool(std::size_t n) {
  Initialize(n);
}

ThreadPool::ThreadPool(LateinitTag) {}

ThreadPool::~ThreadPool() {
  if (stop_) {
    return;
  }
  Terminate();
}

void ThreadPool::Initialize(std::size_t n) {
  workers_.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    workers_.emplace_back([this, this_thread_idx = i] {
      tl_pooled_thread_idx = this_thread_idx;
      while (true) {
        std::function<void()> task;
        {
          std::unique_lock lock(mtx_);
          cv_.wait(lock, [this] {
            return stop_ || !tasks_.empty();
          });
          if (stop_ && tasks_.empty()) {
            return;
          }
          task = std::move(tasks_.front());
          tasks_.pop();
        }
        task();
      }
    });
  }
}

void ThreadPool::Terminate() {
  {
    std::lock_guard lock(mtx_);
    stop_ = true;
  }
  cv_.notify_all();
  for (auto& t : workers_) {
    t.join();
  }
}

std::size_t ThreadPool::CurrentThreadIndex() {
  return tl_pooled_thread_idx;
}

}  // namespace vanadium::lib::concurrency
