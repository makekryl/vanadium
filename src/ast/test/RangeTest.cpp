#include <gtest/gtest.h>

#include "vanadium/ast/ASTTypes.h"

using namespace vanadium;
using namespace vanadium::ast;

TEST(Range, ContainsPos) {
  const Range range{.begin = 10, .end = 20};
  ASSERT_EQ(range.Length(), 10);

  EXPECT_FALSE(range.Contains(9));
  for (pos_t p = 10; p <= 20; ++p) {
    EXPECT_TRUE(range.Contains(p));
  }
  EXPECT_FALSE(range.Contains(21));
}

TEST(Range, ContainsPos_EmptyRange) {
  const Range range{.begin = 20, .end = 20};
  ASSERT_EQ(range.Length(), 0);

  for (pos_t p = 9; p <= 19; ++p) {
    EXPECT_FALSE(range.Contains(p));
  }
  EXPECT_TRUE(range.Contains(20));
  for (pos_t p = 21; p <= 21; ++p) {
    EXPECT_FALSE(range.Contains(p));
  }
}

TEST(Range, ContainsRange) {
  const Range range{.begin = 10, .end = 20};
  ASSERT_EQ(range.Length(), 10);

  EXPECT_TRUE(range.Contains(range));

  EXPECT_FALSE(range.Contains(Range{.begin = 9, .end = 20}));
  EXPECT_FALSE(range.Contains(Range{.begin = 10, .end = 21}));
  //
  EXPECT_FALSE(range.Contains(Range{.begin = 2, .end = 9}));
  EXPECT_FALSE(range.Contains(Range{.begin = 21, .end = 30}));

  EXPECT_TRUE(range.Contains(Range{.begin = 11, .end = 20}));
  EXPECT_TRUE(range.Contains(Range{.begin = 10, .end = 19}));

  EXPECT_TRUE(range.Contains(Range{.begin = 15, .end = 15}));
  EXPECT_TRUE(range.Contains(Range{.begin = 12, .end = 18}));
}

TEST(Range, ContainsRange_EmptyRange) {
  const Range range{.begin = 20, .end = 20};
  ASSERT_EQ(range.Length(), 0);

  EXPECT_TRUE(range.Contains(Range{.begin = 20, .end = 20}));

  EXPECT_FALSE(range.Contains(Range{.begin = 15, .end = 15}));
  EXPECT_FALSE(range.Contains(Range{.begin = 12, .end = 18}));
}
