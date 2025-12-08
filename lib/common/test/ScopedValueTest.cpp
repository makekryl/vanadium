#include <gtest/gtest.h>

#include "ScopedValue.h"

using namespace vanadium::lib;

TEST(ScopedValue, Nested) {
  int i = 1;
  ASSERT_EQ(i, 1);

  {
    ScopedValue s2(i, 2);
    ASSERT_EQ(i, 2);

    {
      ScopedValue s3(i, 3);
      ASSERT_EQ(i, 3);

      {
        ScopedValue s4(i, 4);
        ASSERT_EQ(i, 4);

        {
          ScopedValue s5(i, 5);
          ASSERT_EQ(i, 5);
        }

        EXPECT_EQ(i, 4);
      }

      EXPECT_EQ(i, 3);
    }

    EXPECT_EQ(i, 2);
  }

  EXPECT_EQ(i, 1);
}
