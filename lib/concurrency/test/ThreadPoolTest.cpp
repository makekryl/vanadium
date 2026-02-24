#include <gtest/gtest.h>

#include <future>

#include "vanadium/lib/concurrency/ThreadPool.h"

using namespace vanadium::lib::concurrency;

namespace {
void ExpectPromiseResolution(std::promise<void>& p) {
  auto fut = p.get_future();

  const auto status = fut.wait_for(std::chrono::seconds(3));
  EXPECT_EQ(status, std::future_status::ready);
}
}  // namespace

TEST(ThreadPoolTest, SubmitTask) {
  ThreadPool pool(2);
  std::promise<void> task_done;

  pool.Submit([&task_done]() {
    task_done.set_value();
  });

  ExpectPromiseResolution(task_done);
}

TEST(ThreadPoolTest, SubmitMultipleTasks) {
  constexpr std::size_t kTasks = 10;

  ThreadPool pool(4);
  std::vector<std::promise<void>> promises(kTasks);

  for (std::size_t i = 0; i < kTasks; ++i) {
    pool.Submit([&p = promises[i]]() {
      p.set_value();
    });
  }

  for (auto& p : promises) {
    ExpectPromiseResolution(p);
  }
}

TEST(ThreadPoolTest, CurrentThreadIndexInsideTask) {
  ThreadPool pool(3);
  std::promise<std::size_t> promise;

  pool.Submit([&promise]() {
    promise.set_value(ThreadPool::CurrentThreadIndex());
  });

  std::size_t index = promise.get_future().get();
  EXPECT_GE(index, 0);
  EXPECT_LT(index, pool.Concurrency());
}

TEST(ThreadPoolTest, HighConcurrency) {
  constexpr std::size_t kTasks = 100;
  ThreadPool pool(8);

  std::vector<std::promise<void>> promises(kTasks);

  for (std::size_t i = 0; i < kTasks; ++i) {
    pool.Submit([&p = promises[i]]() {
      p.set_value();
    });
  }

  for (auto& p : promises) {
    ExpectPromiseResolution(p);
  }
}
