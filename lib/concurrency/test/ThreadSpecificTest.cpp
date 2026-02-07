#include <gtest/gtest.h>

#include <atomic>
#include <barrier>
#include <thread>
#include <unordered_set>
#include <vector>

#include "vanadium/lib/concurrency/ThreadSpecific.h"

using namespace vanadium::lib::concurrency;

struct TestObj {
  int value = 0;

  TestObj() {
    constructions_++;
  }
  ~TestObj() {
    destructions_++;
  }

  static std::atomic<std::size_t> constructions_;
  static std::atomic<std::size_t> destructions_;
};
std::atomic<std::size_t> TestObj::constructions_{0};
std::atomic<std::size_t> TestObj::destructions_{0};

TEST(ThreadSpecificTest, SingleThreadSameObject) {
  ThreadSpecific<TestObj> tls;

  auto& obj1 = tls.Local();
  auto& obj2 = tls.Local();

  EXPECT_EQ(&obj1, &obj2);
}

TEST(ThreadSpecificTest, MultipleThreadsUniqueObjects) {
  constexpr std::size_t kThreads = 10;

  ThreadSpecific<TestObj> tls;
  std::vector<std::unordered_set<TestObj*>> thread_instances(kThreads);

  {
    std::barrier sync_point(kThreads);

    std::vector<std::jthread> threads;
    threads.reserve(kThreads);
    for (std::size_t i = 0; i < kThreads; ++i) {
      threads.emplace_back(
          [&tls, &sync_point](std::unordered_set<TestObj*>& instances) {
            auto& obj = tls.Local();
            obj.value = 42;
            instances.insert(&obj);

            sync_point.arrive_and_wait();  // to prevent deallocation
          },
          std::ref(thread_instances[i]));
    }
  }

  std::unordered_set<TestObj*> all_objects;
  for (const auto& s : thread_instances) {
    ASSERT_EQ(s.size(), 1);
    all_objects.insert(*s.begin());
  }

  EXPECT_EQ(all_objects.size(), kThreads);
}

TEST(ThreadSpecificTest, ConstructionDestructionCount) {
  constexpr std::size_t kThreads = 10;

  TestObj::constructions_ = 0;
  TestObj::destructions_ = 0;

  {
    ThreadSpecific<TestObj> tls;
    std::vector<std::jthread> threads;
    threads.reserve(kThreads);
    for (std::size_t i = 0; i < kThreads; ++i) {
      threads.emplace_back([i, &tls]() {
        auto& obj = tls.Local();
        obj.value = i;
      });
    }
  }

  EXPECT_EQ(TestObj::constructions_.load(), kThreads);
  EXPECT_EQ(TestObj::destructions_.load(), kThreads);
}
