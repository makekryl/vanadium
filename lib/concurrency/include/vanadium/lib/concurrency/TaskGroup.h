#pragma once

#include "vanadium/lib/concurrency/TaskArena.h"

namespace vanadium::lib::concurrency {

class TaskGroup {
 public:
  template <std::invocable F>
  void Run(F&& f) {
    if (cancelled_.load(std::memory_order_acquire)) {
      return;
    }

    active_tasks_.fetch_add(1, std::memory_order_acq_rel);

    TaskArena::Current().Enqueue([this, func = std::forward<F>(f)]() mutable {
#if __cpp_exceptions
      try {
        if (!cancelled_.load(std::memory_order_acquire)) {
          func();
        }
      } catch (...) {
        std::lock_guard l(exc_mtx_);
        exc_ = std::current_exception();
        if (exc_) {
          std::rethrow_exception(exc_);
        }
      }
#else
      if (!cancelled_.load(std::memory_order_acquire)) {
        func();
      }
#endif

      if (active_tasks_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
        active_tasks_.notify_all();
      }
    });
  }

  void Wait() {
    std::size_t expected;
    while ((expected = active_tasks_.load(std::memory_order_acquire)) != 0) {
      active_tasks_.wait(expected, std::memory_order_acquire);
    }

#if __cpp_exceptions
    if (exc_) {
      std::rethrow_exception(exc_);
    }
#endif
  }

  void Cancel() noexcept {
    cancelled_.store(true, std::memory_order_release);
  }

 private:
  std::atomic<std::size_t> active_tasks_{0};
  std::atomic<bool> cancelled_{false};

#if __cpp_exceptions
  std::exception_ptr exc_{nullptr};
  std::mutex exc_mtx_;
#endif
};

}  // namespace vanadium::lib::concurrency
