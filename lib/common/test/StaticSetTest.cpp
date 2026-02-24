#include <gtest/gtest.h>

#include "vanadium/lib/StaticSet.h"

using namespace vanadium::lib;

TEST(StaticSetTest, Contains) {
  constexpr auto kSet = MakeStaticSet<int>({3, 1, 4, 2});

  EXPECT_TRUE(kSet.contains(1));
  EXPECT_TRUE(kSet.contains(2));
  EXPECT_TRUE(kSet.contains(3));
  EXPECT_TRUE(kSet.contains(4));

  EXPECT_FALSE(kSet.contains(42));
}
