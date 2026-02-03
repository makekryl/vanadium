#pragma once

#include <concepts>

#include <vanadium/lib/ScopedValue.h>

#include "vanadium/lib/concurrency/ThreadPool.h"

namespace vanadium::lib::concurrency {

class TaskArena {
 public:
  TaskArena(std::size_t concurrency) : pool_(concurrency) {
    current_arena_ = this;
  }

  [[nodiscard]] std::size_t Concurrency() const noexcept(noexcept(pool_.Concurrency())) {
    return pool_.Concurrency();
  }

  void Terminate() {
    pool_.Terminate();
  }

  void Enqueue(std::invocable auto&& f) {
    lib::ScopedValue guard(current_arena_, this);
    pool_.Submit(f);
  }

  auto Execute(std::invocable auto&& f) -> decltype(f()) {
    lib::ScopedValue guard(current_arena_, this);
    return f();
  }

  static TaskArena& Current();

 private:
  ThreadPool pool_;

  static thread_local TaskArena* current_arena_;

  TaskArena(ThreadPool::LateinitTag) : pool_(ThreadPool::LateinitTag{}) {}
  static TaskArena global_arena_;
};

}  // namespace vanadium::lib::concurrency
