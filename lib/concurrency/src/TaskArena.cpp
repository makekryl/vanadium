#include "vanadium/lib/concurrency/TaskArena.h"

#include <thread>

namespace vanadium::lib::concurrency {

thread_local TaskArena* TaskArena::current_arena_{nullptr};
TaskArena TaskArena::global_arena_;

TaskArena& TaskArena::Current() {
  if (!current_arena_) [[unlikely]] {
    if (!global_arena_.pool_) {
      global_arena_.pool_.emplace(std::thread::hardware_concurrency());
    };
    return global_arena_;
  }
  return *current_arena_;
}

}  // namespace vanadium::lib::concurrency
