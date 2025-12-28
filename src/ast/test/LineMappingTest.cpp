#include <gtest/gtest.h>

#include "vanadium/ast/ASTTypes.h"

using namespace vanadium;
using namespace vanadium::ast;

TEST(LineMapping, StartOf) {
  const LineMapping lm(std::vector<pos_t>{0, 5, 15, 24, 30});
  ASSERT_EQ(lm.Count(), 5);

  EXPECT_EQ(lm.StartOf(0), 0);
  EXPECT_EQ(lm.StartOf(1), 5);
  EXPECT_EQ(lm.StartOf(2), 15);
  EXPECT_EQ(lm.StartOf(3), 24);
  EXPECT_EQ(lm.StartOf(4), 30);
}

TEST(LineMapping, StartOf_Oneline) {
  const LineMapping lm(std::vector<pos_t>{0});
  ASSERT_EQ(lm.Count(), 1);

  EXPECT_EQ(lm.StartOf(0), 0);
}

TEST(LineMapping, LineOf) {
  const LineMapping lm(std::vector<pos_t>{0, 5, 15, 24, 30});
  ASSERT_EQ(lm.Count(), 5);

  for (pos_t p = 0; p < 5; ++p) {
    EXPECT_EQ(lm.LineOf(p), 0);
  }
  for (pos_t p = 5; p < 15; ++p) {
    EXPECT_EQ(lm.LineOf(p), 1);
  }
  for (pos_t p = 15; p < 24; ++p) {
    EXPECT_EQ(lm.LineOf(p), 2);
  }
  for (pos_t p = 24; p < 30; ++p) {
    EXPECT_EQ(lm.LineOf(p), 3);
  }
  for (pos_t p = 30; p < 50; ++p) {
    EXPECT_EQ(lm.LineOf(p), 4);
  }
}

TEST(LineMapping, LineOf_Oneline) {
  const LineMapping lm(std::vector<pos_t>{0});
  ASSERT_EQ(lm.Count(), 1);

  for (pos_t p = 0; p < 50; ++p) {
    EXPECT_EQ(lm.LineOf(p), 0);
  }
}

TEST(LineMapping, Translate) {
  const LineMapping lm(std::vector<pos_t>{0, 5, 15, 24, 30});
  ASSERT_EQ(lm.Count(), 5);

  for (pos_t p = 0; p < 5; ++p) {
    const Location expected{.line = 0, .column = p};
    EXPECT_EQ(lm.Translate(p), expected);
  }
  for (pos_t p = 5; p < 15; ++p) {
    const Location expected{.line = 1, .column = p - 5};
    EXPECT_EQ(lm.Translate(p), expected);
  }
  for (pos_t p = 15; p < 24; ++p) {
    const Location expected{.line = 2, .column = p - 15};
    EXPECT_EQ(lm.Translate(p), expected);
  }
  for (pos_t p = 24; p < 30; ++p) {
    const Location expected{.line = 3, .column = p - 24};
    EXPECT_EQ(lm.Translate(p), expected);
  }
  for (pos_t p = 30; p < 50; ++p) {
    const Location expected{.line = 4, .column = p - 30};
    EXPECT_EQ(lm.Translate(p), expected);
  }
}

TEST(LineMapping, Translate_Oneline) {
  const LineMapping lm(std::vector<pos_t>{0});
  ASSERT_EQ(lm.Count(), 1);

  for (pos_t p = 0; p < 50; ++p) {
    const Location expected{.line = 0, .column = p};
    EXPECT_EQ(lm.Translate(p), expected);
  }
}

TEST(LineMapping, GetPosition) {
  const LineMapping lm(std::vector<pos_t>{0, 5, 15, 24, 30});
  ASSERT_EQ(lm.Count(), 5);

  for (pos_t p = 0; p < 5; ++p) {
    const Location loc{.line = 0, .column = p};
    EXPECT_EQ(lm.GetPosition(loc), p);
  }
  for (pos_t p = 5; p < 15; ++p) {
    const Location loc{.line = 1, .column = p - 5};
    EXPECT_EQ(lm.GetPosition(loc), p);
  }
  for (pos_t p = 15; p < 24; ++p) {
    const Location loc{.line = 2, .column = p - 15};
    EXPECT_EQ(lm.GetPosition(loc), p);
  }
  for (pos_t p = 24; p < 30; ++p) {
    const Location loc{.line = 3, .column = p - 24};
    EXPECT_EQ(lm.GetPosition(loc), p);
  }
  for (pos_t p = 30; p < 50; ++p) {
    const Location loc{.line = 4, .column = p - 30};
    EXPECT_EQ(lm.GetPosition(loc), p);
  }
}

TEST(LineMapping, GetPosition_Oneline) {
  const LineMapping lm(std::vector<pos_t>{0});
  ASSERT_EQ(lm.Count(), 1);

  for (pos_t p = 0; p < 50; ++p) {
    const Location loc{.line = 0, .column = p};
    EXPECT_EQ(lm.GetPosition(loc), p);
  }
}

TEST(LineMapping, RangeOf) {
  const LineMapping lm(std::vector<pos_t>{0, 5, 15, 24, 30});
  ASSERT_EQ(lm.Count(), 5);

  {
    const Range expected{.begin = 0, .end = 4};
    EXPECT_EQ(lm.RangeOf(0), expected);
  }
  {
    const Range expected{.begin = 5, .end = 14};
    EXPECT_EQ(lm.RangeOf(1), expected);
  }
  {
    const Range expected{.begin = 15, .end = 23};
    EXPECT_EQ(lm.RangeOf(2), expected);
  }
  {
    const Range expected{.begin = 24, .end = 29};
    EXPECT_EQ(lm.RangeOf(3), expected);
  }
  {
    const Range expected{.begin = 30, .end = static_cast<pos_t>(-1)};
    EXPECT_EQ(lm.RangeOf(4), expected);
  }
}

TEST(LineMapping, RangeOf_Oneline) {
  const LineMapping lm(std::vector<pos_t>{0});
  ASSERT_EQ(lm.Count(), 1);

  {
    const Range expected{.begin = 0, .end = static_cast<pos_t>(-1)};
    EXPECT_EQ(lm.RangeOf(0), expected);
  }
}
