#include <gtest/gtest.h>

#include "vanadium/lib/Bitset.h"

namespace {

using namespace vanadium::lib;

class BitsetTest : public ::testing::Test {
 protected:
  static constexpr std::size_t kHeapSize = (Bitset::kCellBits * 1024) + 3;

  static void SetPattern(Bitset& bs) {
    if (bs.Size() == 0) {
      return;
    }

    bs.Set(0);
    bs.Set(bs.Size() / 2);
    bs.Set(bs.Size() - 1);

    if (bs.Size() > Bitset::kCellBits) {
      bs.Set(Bitset::kCellBits - 1);
      bs.Set(Bitset::kCellBits);
    }
  }

  static void ExpectEqual(const Bitset& lhs, const Bitset& rhs) {
    ASSERT_EQ(lhs.Size(), rhs.Size());

    for (std::size_t i = 0; i < lhs.Size(); ++i) {
      EXPECT_EQ(lhs.Get(i), rhs.Get(i)) << "Bit index: " << i;
    }

    EXPECT_EQ(lhs.ActiveCount(), rhs.ActiveCount());
    EXPECT_EQ(lhs.InactiveCount(), rhs.InactiveCount());
    EXPECT_EQ(lhs.All(), rhs.All());
  }

  static void ExpectEmpty(const Bitset& bs) {
    EXPECT_EQ(bs.Size(), 0);
    EXPECT_EQ(bs.ActiveCount(), 0);
    EXPECT_EQ(bs.InactiveCount(), 0);
    EXPECT_TRUE(bs.All());
  }
};

TEST_F(BitsetTest, DefaultConstructedIsEmpty) {
  Bitset bs;

  EXPECT_EQ(bs.Size(), 0);
  EXPECT_EQ(bs.ActiveCount(), 0);
  EXPECT_EQ(bs.InactiveCount(), 0);
  EXPECT_TRUE(bs.All());
}

TEST_F(BitsetTest, ConstructWithSize) {
  Bitset bs(60);

  EXPECT_EQ(bs.Size(), 60);
  EXPECT_EQ(bs.ActiveCount(), 0);
  EXPECT_EQ(bs.InactiveCount(), 60);
  EXPECT_FALSE(bs.All());
}

TEST_F(BitsetTest, ConstructWithSizeMulticell) {
  Bitset bs(70);

  EXPECT_EQ(bs.Size(), 70);
  EXPECT_EQ(bs.ActiveCount(), 0);
  EXPECT_EQ(bs.InactiveCount(), 70);
  EXPECT_FALSE(bs.All());
}

TEST_F(BitsetTest, SetAndGetSingleBit) {
  Bitset bs(10);
  ASSERT_FALSE(bs.Get(3));
  ASSERT_EQ(bs.ActiveCount(), 0);

  bs.Set(3);
  EXPECT_TRUE(bs.Get(3));
  EXPECT_EQ(bs.ActiveCount(), 1);
}

TEST_F(BitsetTest, ClearBit) {
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

TEST_F(BitsetTest, Multicell) {
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

TEST_F(BitsetTest, And) {
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

TEST_F(BitsetTest, Or) {
  Bitset a(8);
  Bitset b(8);

  a.Set(1);
  b.Set(3);

  a |= b;

  EXPECT_TRUE(a.Get(1));
  EXPECT_TRUE(a.Get(3));
  EXPECT_EQ(a.ActiveCount(), 2);
}

TEST_F(BitsetTest, Flip) {
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

TEST_F(BitsetTest, Reset) {
  Bitset bs(20);

  bs.Set(5);
  bs.Set(10);

  bs.Reset();

  EXPECT_EQ(bs.ActiveCount(), 0);
  EXPECT_EQ(bs.InactiveCount(), 20);
}

TEST_F(BitsetTest, All) {
  Bitset bs(5);

  for (std::size_t i = 0; i < 5; ++i) {
    bs.Set(i);
  }

  EXPECT_TRUE(bs.All());
}

TEST_F(BitsetTest, CopyConstructSmall) {
  Bitset original(60);
  SetPattern(original);

  Bitset copy(original);

  ExpectEqual(copy, original);

  original.Reset();

  EXPECT_EQ(copy.ActiveCount(), 3);
}

TEST_F(BitsetTest, CopyConstructHeap) {
  Bitset original(kHeapSize);
  SetPattern(original);

  Bitset copy(original);

  ExpectEqual(copy, original);

  original.Reset();

  EXPECT_GT(copy.ActiveCount(), 0);
}

TEST_F(BitsetTest, CopyAssignSmall) {
  Bitset original(60);
  SetPattern(original);

  Bitset copy;
  copy = original;

  ExpectEqual(copy, original);

  original.Reset();

  EXPECT_EQ(copy.ActiveCount(), 3);
}

TEST_F(BitsetTest, CopyAssignHeap) {
  Bitset original(kHeapSize);
  SetPattern(original);

  Bitset copy;
  copy = original;

  ExpectEqual(copy, original);

  original.Reset();

  EXPECT_GT(copy.ActiveCount(), 0);
}

TEST_F(BitsetTest, CopyAssignAcrossStorageTypes) {
  Bitset small(60);
  Bitset large(kHeapSize);

  SetPattern(small);
  SetPattern(large);

  // sso -> heap
  Bitset heap_destination(kHeapSize);
  heap_destination = small;
  ExpectEqual(heap_destination, small);

  // heap -> sso
  Bitset small_destination(60);
  small_destination = large;
  ExpectEqual(small_destination, large);
}

TEST_F(BitsetTest, MoveConstructSmall) {
  Bitset original(60);
  SetPattern(original);

  Bitset expected(original);
  Bitset moved(std::move(original));

  ExpectEqual(moved, expected);
  ExpectEmpty(original);
}

TEST_F(BitsetTest, MoveConstructHeap) {
  Bitset original(kHeapSize);
  SetPattern(original);

  Bitset expected(original);
  Bitset moved(std::move(original));

  ExpectEqual(moved, expected);
  ExpectEmpty(original);
}

TEST_F(BitsetTest, MoveAssignSmallToSmall) {
  Bitset original(60);
  SetPattern(original);

  Bitset expected(original);
  Bitset destination(30);

  destination = std::move(original);

  ExpectEqual(destination, expected);
  ExpectEmpty(original);
}

TEST_F(BitsetTest, MoveAssignSmallToHeap) {
  Bitset original(60);
  SetPattern(original);

  Bitset expected(original);
  Bitset destination(kHeapSize);

  destination = std::move(original);

  ExpectEqual(destination, expected);
  ExpectEmpty(original);
}

TEST_F(BitsetTest, MoveAssignHeapToSmall) {
  Bitset original(kHeapSize);
  SetPattern(original);

  Bitset expected(original);
  Bitset destination(60);

  destination = std::move(original);

  ExpectEqual(destination, expected);
  ExpectEmpty(original);
}

TEST_F(BitsetTest, MoveAssignHeapToHeap) {
  Bitset original(kHeapSize);
  SetPattern(original);

  Bitset expected(original);
  Bitset destination(kHeapSize);

  destination.Set(100);

  destination = std::move(original);

  ExpectEqual(destination, expected);
  ExpectEmpty(original);
}

TEST_F(BitsetTest, FlipMulticell) {
  Bitset bs(kHeapSize);

  bs.Set(0);
  bs.Set(Bitset::kCellBits - 1);
  bs.Set(Bitset::kCellBits);
  bs.Set(kHeapSize - 1);

  bs.Flip();

  EXPECT_EQ(bs.ActiveCount(), kHeapSize - 4);
  EXPECT_EQ(bs.InactiveCount(), 4);

  EXPECT_FALSE(bs.Get(0));
  EXPECT_FALSE(bs.Get(Bitset::kCellBits - 1));
  EXPECT_FALSE(bs.Get(Bitset::kCellBits));
  EXPECT_FALSE(bs.Get(kHeapSize - 1));

  bs.Flip();

  EXPECT_EQ(bs.ActiveCount(), 4);
  EXPECT_TRUE(bs.Get(0));
  EXPECT_TRUE(bs.Get(Bitset::kCellBits - 1));
  EXPECT_TRUE(bs.Get(Bitset::kCellBits));
  EXPECT_TRUE(bs.Get(kHeapSize - 1));
}

TEST_F(BitsetTest, ResetHeapStorage) {
  Bitset bs(kHeapSize);
  SetPattern(bs);

  bs.Reset();

  EXPECT_EQ(bs.ActiveCount(), 0);
  EXPECT_EQ(bs.InactiveCount(), kHeapSize);
  EXPECT_FALSE(bs.All());
}

}  // namespace
