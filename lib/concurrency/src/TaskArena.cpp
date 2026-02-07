#include "vanadium/lib/concurrency/TaskArena.h"

#include <mutex>
#include <thread>

#include "vanadium/lib/concurrency/ThreadPool.h"

namespace vanadium::lib::concurrency {

thread_local TaskArena* TaskArena::current_arena_{nullptr};

TaskArena TaskArena::global_arena_;
namespace {
std::once_flag global_task_arena_init_flag;
}

TaskArena& TaskArena::Current() {
  if (!current_arena_) [[unlikely]] {
    std::call_once(global_task_arena_init_flag, [&] {
      global_arena_.pool_.Initialize(std::thread::hardware_concurrency());
    });
    return global_arena_;
  }
  return *current_arena_;
}

}  // namespace vanadium::lib::concurrency
