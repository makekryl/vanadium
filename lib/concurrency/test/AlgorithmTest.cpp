#include <gtest/gtest.h>

#include <algorithm>
#include <numeric>
#include <vector>

#include "vanadium/lib/concurrency/Algorithm.h"

using namespace vanadium::lib::concurrency;

TEST(ParallelForTest, Sum) {
  constexpr int kN = 10000;
  std::vector<int> data(kN);
  std::ranges::iota(data, 1);

  std::atomic<int> sum{0};

  ParallelFor(data, [&](int v) {
    sum.fetch_add(v, std::memory_order_relaxed);
  });

  EXPECT_EQ(sum.load(), (kN * (kN + 1)) / 2);
}

TEST(ParallelForTest, EachElementProcessedExactlyOnce) {
  std::vector<int> data(300);
  std::ranges::iota(data, 1);

  std::mutex m;
  std::set<int> visited;

  ParallelFor(data, [&](int v) {
    std::lock_guard l(m);
    visited.insert(v);
  });

  EXPECT_EQ(visited.size(), data.size());
  for (int v : data) {
    EXPECT_TRUE(visited.contains(v));
  }
}
