#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <future>
#include <latch>
#include <thread>

#include "vanadium/lib/concurrency/ConcurrentQueue.h"

using namespace vanadium::lib::concurrency;

//

TEST(ConcurrentQueueTest, PushAndTryPop) {
  ConcurrentQueue<int> q;

  q.push(42);

  EXPECT_FALSE(q.empty());
  EXPECT_EQ(q.size(), 1);

  int value{};
  EXPECT_TRUE(q.try_pop(value));
  EXPECT_EQ(value, 42);
  EXPECT_TRUE(q.empty());
}

TEST(ConcurrentQueueTest, TryPopEmptyReturnsFalse) {
  ConcurrentQueue<int> q;

  int value{};
  EXPECT_FALSE(q.try_pop(value));
}

//

TEST(ConcurrentQueueTest, PopBlocksUntilValueAvailable) {
  ConcurrentQueue<int> q;

  std::promise<int> result_promise;
  auto result_future = result_promise.get_future();

  std::jthread consumer([&] {
    auto&& value = q.pop();
    result_promise.set_value(value);
  });

  std::this_thread::sleep_for(std::chrono::microseconds{500});
  q.push(42);

  EXPECT_EQ(result_future.get(), 42);
}

TEST(ConcurrentQueueTest, MultipleProducers) {
  constexpr int kProducerCount = 4;
  constexpr int kValuesPerProducer = 1000;

  ConcurrentQueue<int> q;

  {
    std::latch sp(kProducerCount);
    std::vector<std::jthread> producers;
    producers.reserve(kProducerCount);
    for (int p = 0; p < kProducerCount; ++p) {
      producers.emplace_back([&q, p, &sp] {
        sp.arrive_and_wait();
        for (int i = 0; i < kValuesPerProducer; ++i) {
          q.push((p * kValuesPerProducer) + i);
        }
      });
    }
  }

  EXPECT_EQ(q.size(), kProducerCount * kValuesPerProducer);
}

TEST(ConcurrentQueueTest, MultipleConsumers) {
  constexpr int kItemCount = 10000;
  constexpr int kConsumerCount = 4;

  ConcurrentQueue<int> q;

  for (int i = 0; i < kItemCount; ++i) {
    q.push(i);
  }

  std::atomic<int> consumed_count = 0;
  {
    std::vector<std::jthread> consumers;
    consumers.reserve(kConsumerCount);
    for (int c = 0; c < kConsumerCount; ++c) {
      consumers.emplace_back([&] {
        while (true) {
          int value;
          if (q.try_pop(value)) {
            consumed_count++;
          } else {
            break;
          }
        }
      });
    }
  }

  EXPECT_EQ(consumed_count.load(), kItemCount);
  EXPECT_TRUE(q.empty());
}

TEST(ConcurrentQueueTest, ProducerConsumerStress) {
  constexpr int kTotalItems = 50000;

  ConcurrentQueue<int> q;

  std::atomic<int> consumed = 0;
  {
    std::jthread producer([&] {
      for (int i = 0; i < kTotalItems; ++i) {
        q.push(i);
      }
    });
    std::jthread consumer([&] {
      for (int i = 0; i < kTotalItems; ++i) {
        q.pop();
        consumed++;
      }
    });
  }

  EXPECT_EQ(consumed.load(), kTotalItems);
  EXPECT_TRUE(q.empty());
}
