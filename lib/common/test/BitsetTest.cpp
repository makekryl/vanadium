#include <gtest/gtest.h>

#include "vanadium/lib/Bitset.h"

using namespace vanadium::lib;

TEST(BitsetTest, DefaultConstructedIsEmpty) {
  Bitset bs;

  EXPECT_EQ(bs.Size(), 0);
  EXPECT_EQ(bs.ActiveCount(), 0);
  EXPECT_EQ(bs.InactiveCount(), 0);
  EXPECT_TRUE(bs.All());
}

TEST(BitsetTest, ConstructWithSize) {
  Bitset bs(60);

  EXPECT_EQ(bs.Size(), 60);
  EXPECT_EQ(bs.ActiveCount(), 0);
  EXPECT_EQ(bs.InactiveCount(), 60);
  EXPECT_FALSE(bs.All());
}

TEST(BitsetTest, ConstructWithSizeMulticell) {
  Bitset bs(70);

  EXPECT_EQ(bs.Size(), 70);
  EXPECT_EQ(bs.ActiveCount(), 0);
  EXPECT_EQ(bs.InactiveCount(), 70);
  EXPECT_FALSE(bs.All());
}

TEST(BitsetTest, SetAndGetSingleBit) {
  Bitset bs(10);
  ASSERT_FALSE(bs.Get(3));
  ASSERT_EQ(bs.ActiveCount(), 0);

  bs.Set(3);
  EXPECT_TRUE(bs.Get(3));
  EXPECT_EQ(bs.ActiveCount(), 1);
}

TEST(BitsetTest, ClearBit) {
  Bitset bs(10);
  ASSERT_FALSE(bs.Get(5));
  ASSERT_EQ(bs.ActiveCount(), 0);

  bs.Set(5);
  ASSERT_TRUE(bs.Get(5));
  ASSERT_EQ(bs.ActiveCount(), 1);

  bs.Clear(5);
  EXPECT_FALSE(bs.Get(5));
  EXPECT_EQ(bs.ActiveCount(), 0);
}

TEST(BitsetTest, Multicell) {
  constexpr std::size_t kSize = Bitset::kCellBits + 5;
  Bitset bs(kSize);

  bs.Set(0);
  EXPECT_TRUE(bs.Get(0));

  bs.Set(Bitset::kCellBits - 1);
  EXPECT_TRUE(bs.Get(Bitset::kCellBits - 1));

  bs.Set(Bitset::kCellBits);
  EXPECT_TRUE(bs.Get(Bitset::kCellBits));

  bs.Set(kSize - 1);
  EXPECT_TRUE(bs.Get(kSize - 1));

  EXPECT_EQ(bs.ActiveCount(), 4);
}

TEST(BitsetTest, And) {
  Bitset a(8);
  Bitset b(8);

  a.Set(1);
  a.Set(2);

  b.Set(2);
  b.Set(3);

  a &= b;

  EXPECT_FALSE(a.Get(1));
  EXPECT_TRUE(a.Get(2));
  EXPECT_FALSE(a.Get(3));

  EXPECT_EQ(a.ActiveCount(), 1);
}

TEST(BitsetTest, Or) {
  Bitset a(8);
  Bitset b(8);

  a.Set(1);
  b.Set(3);

  a |= b;

  EXPECT_TRUE(a.Get(1));
  EXPECT_TRUE(a.Get(3));
  EXPECT_EQ(a.ActiveCount(), 2);
}

TEST(BitsetTest, Flip) {
  Bitset bs(7);

  bs.Set(1);
  bs.Set(3);

  ASSERT_TRUE(bs.Get(1));
  ASSERT_TRUE(bs.Get(3));

  bs.Flip();

  EXPECT_FALSE(bs.Get(1));
  EXPECT_FALSE(bs.Get(3));

  EXPECT_TRUE(bs.Get(0));
  EXPECT_TRUE(bs.Get(2));
  EXPECT_TRUE(bs.Get(4));
  EXPECT_TRUE(bs.Get(5));
  EXPECT_TRUE(bs.Get(6));

  EXPECT_EQ(bs.ActiveCount(), 5);
  EXPECT_EQ(bs.InactiveCount(), 2);

  EXPECT_FALSE(bs.All());
}

TEST(BitsetTest, Reset) {
  Bitset bs(20);

  bs.Set(5);
  bs.Set(10);

  bs.Reset();

  EXPECT_EQ(bs.ActiveCount(), 0);
  EXPECT_EQ(bs.InactiveCount(), 20);
}

TEST(BitsetTest, All) {
  Bitset bs(5);

  for (std::size_t i = 0; i < 5; ++i) {
    bs.Set(i);
  }

  EXPECT_TRUE(bs.All());
}
