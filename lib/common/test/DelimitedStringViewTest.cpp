#include <gtest/gtest.h>

#include "vanadium/lib/DelimitedStringView.h"

using namespace vanadium::lib;

class DelimitedStringViewTest : public ::testing::Test {
 protected:
  template <char Delimiter>
  std::vector<std::string> CollectSegments(const DelimitedStringView<Delimiter>& dsv) {
    std::vector<std::string> segments;
    for (auto s : dsv.range()) {
      segments.emplace_back(s);
    }
    return segments;
  }
};

TEST_F(DelimitedStringViewTest, EmptyString) {
  EXPECT_TRUE(CollectSegments(DelimitedStringView<','>("")).empty());
}

TEST_F(DelimitedStringViewTest, SingleSegment) {
  EXPECT_EQ(CollectSegments(DelimitedStringView<','>("Hello")), std::vector<std::string>{"Hello"});
}

TEST_F(DelimitedStringViewTest, MultipleSegments) {
  EXPECT_EQ(CollectSegments(DelimitedStringView<','>("Hello,World,Test")),
            (std::vector<std::string>{"Hello", "World", "Test"}));
}

TEST_F(DelimitedStringViewTest, LeadingTrailingSpaces) {
  EXPECT_EQ(CollectSegments(DelimitedStringView<','>("  Hello ,  World , Test")),
            (std::vector<std::string>{"Hello", "World", "Test"}));
}
