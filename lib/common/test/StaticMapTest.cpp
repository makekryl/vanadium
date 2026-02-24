#include <gtest/gtest.h>

#include "vanadium/lib/StaticMap.h"

using namespace vanadium::lib;

TEST(StaticMapTest, Get) {
  constexpr auto kMap = MakeStaticMap<int, std::string_view>({
      {1, "1"},
      {3, "3"},
      {2, "2"},
  });

  EXPECT_EQ(kMap[1], "1");
  EXPECT_EQ(kMap[2], "2");
  EXPECT_EQ(kMap[3], "3");

  const auto* val = kMap.get(2);
  ASSERT_NE(val, nullptr);
  EXPECT_EQ(*val, "2");

  EXPECT_EQ(kMap.get(42), nullptr);
}

TEST(StaticMapTest, Order) {
  constexpr auto kMap = MakeStaticMap<int, char>({
      {5, '5'},
      {3, '3'},
      {1, '1'},
      {4, '4'},
      {2, '2'},
  });

  const auto& entries = kMap.Entries();
  std::array<int, 5> expected_keys{1, 2, 3, 4, 5};
  for (std::size_t i = 0; i < entries.size(); ++i) {
    EXPECT_EQ(entries[i].first, expected_keys[i]);
  }
}
