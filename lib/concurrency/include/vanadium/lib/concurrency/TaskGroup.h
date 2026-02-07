#pragma once

#include "vanadium/lib/concurrency/TaskArena.h"

namespace vanadium::lib::concurrency {

class TaskGroup {
 public:
  template <std::invocable F>
  void Run(F&& f) {
    if (cancelled_.load(std::memory_order_relaxed)) {
      return;
    }

    active_tasks_.fetch_add(1, std::memory_order_relaxed);
    TaskArena::Current().Enqueue([this, func = std::forward<F>(f)] {
#if __cpp_exceptions
      try {
        if (!cancelled_.load(std::memory_order_relaxed)) {
          func();
        }
      } catch (...) {
        std::lock_guard<std::mutex> lock(exc_mtx_);
        exc_ = std::current_exception();
      }
#else
      if (!cancelled_.load(std::memory_order_relaxed)) {
        func();
      }
#endif
      active_tasks_.fetch_sub(1, std::memory_order_relaxed);
      cv_.notify_all();
    });
  }

  void Wait() {
    std::unique_lock<std::mutex> lock(wait_mtx_);
    cv_.wait(lock, [this] {
      return active_tasks_.load(std::memory_order_relaxed) == 0;
    });
#if __cpp_exceptions
    if (exc_) {
      std::rethrow_exception(exc_);
    }
#endif
  }

  void Cancel() noexcept {
    cancelled_.store(true, std::memory_order_relaxed);
  }

 private:
  std::atomic<bool> cancelled_{false};
  std::atomic<std::uint16_t> active_tasks_{0};
  std::condition_variable cv_;
  std::mutex wait_mtx_;

#if __cpp_exceptions
  std::exception_ptr exc_;
  std::mutex exc_mtx_;
#endif
};

}  // namespace vanadium::lib::concurrency
