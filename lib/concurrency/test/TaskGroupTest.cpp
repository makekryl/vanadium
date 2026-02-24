#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <thread>

#include "vanadium/lib/concurrency/TaskGroup.h"

using namespace vanadium::lib::concurrency;

TEST(TaskGroupTest, SingleTask) {
  TaskGroup tg;

  std::atomic<bool> executed{false};
  tg.Run([&] {
    executed = true;
  });

  tg.Wait();
  EXPECT_TRUE(executed.load());
}

TEST(TaskGroupTest, MultipleTasks) {
  constexpr int kTasks = 10;

  TaskGroup wg;

  std::atomic<int> counter{0};
  for (int i = 0; i < kTasks; ++i) {
    wg.Run([&] {
      counter.fetch_add(1, std::memory_order_relaxed);
    });
  }

  wg.Wait();
  EXPECT_EQ(counter.load(), kTasks);
}

TEST(TaskGroupTest, WaitBlocksUntilCompletion) {
  TaskGroup tg;
  std::atomic<bool> finished{false};

  tg.Run([&] {
    std::this_thread::sleep_for(std::chrono::microseconds{500});
    finished = true;
  });

  tg.Wait();
  EXPECT_TRUE(finished.load());
}

TEST(TaskGroupTest, CancelPreventsNewTasks) {
  TaskGroup tg;
  std::atomic<bool> executed{false};

  tg.Cancel();

  tg.Run([&] {
    executed = true;
  });

  tg.Wait();
  EXPECT_FALSE(executed.load());
}
