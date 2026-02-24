#include <gtest/gtest.h>

#include <memory>
#include <utility>

#include "vanadium/lib/HeapArray.h"

using namespace vanadium::lib;

TEST(HeapArrayTest, DefaultInitialization) {
  constexpr std::size_t kSize = 5;

  HeapArray<int> arr(kSize);

  ASSERT_EQ(arr.Size(), kSize);
  for (std::size_t i = 0; i < arr.Size(); ++i) {
    EXPECT_EQ(arr[i], 0);
  }
}

TEST(HeapArrayTest, ElementAccessAndModification) {
  HeapArray<int> arr(3);

  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;

  EXPECT_EQ(arr[0], 1);
  EXPECT_EQ(arr[1], 2);
  EXPECT_EQ(arr[2], 3);
}

TEST(HeapArrayTest, CopyConstructor) {
  HeapArray<int> original(3);
  original[0] = 1;
  original[1] = 2;
  original[2] = 3;

  HeapArray<int> copy(original);

  EXPECT_EQ(copy.Size(), original.Size());
  for (std::size_t i = 0; i < original.Size(); ++i) {
    EXPECT_EQ(copy[i], original[i]);
  }

  original[0] = 100;
  EXPECT_EQ(copy[0], 1);
}

TEST(HeapArrayTest, MoveConstructor) {
  constexpr std::size_t kSize = 2;

  HeapArray<int> original(kSize);
  original[0] = 7;
  original[1] = 8;

  auto* p0 = &(original[0]);

  auto moved = std::move(original);

  EXPECT_EQ(moved.Size(), kSize);
  EXPECT_EQ(moved[0], 7);
  EXPECT_EQ(moved[1], 8);

  EXPECT_EQ(&(moved[0]), p0);
}

TEST(HeapArrayTest, CopyAssignment) {
  constexpr std::size_t kSize = 3;

  HeapArray<int> a(kSize);
  a[0] = 1;
  a[1] = 2;
  a[2] = 3;

  HeapArray<int> b(1);
  b = a;

  EXPECT_EQ(b.Size(), kSize);
  for (std::size_t i = 0; i < b.Size(); ++i) {
    EXPECT_EQ(b[i], a[i]);
  }

  a[0] = 99;
  EXPECT_EQ(b[0], 1);
}

TEST(HeapArrayTest, MoveAssignment) {
  constexpr std::size_t kSize = 2;

  HeapArray<int> a(kSize);
  a[0] = 5;
  a[1] = 6;

  HeapArray<int> b(1);
  b = std::move(a);

  EXPECT_EQ(b.Size(), kSize);
  EXPECT_EQ(b[0], 5);
  EXPECT_EQ(b[1], 6);
}

TEST(HeapArrayTest, NonTrivialType) {
  HeapArray<std::shared_ptr<int>> arr(2);
  arr[0] = std::make_shared<int>(1);
  arr[1] = std::make_shared<int>(2);

  HeapArray<std::shared_ptr<int>> copy(arr);

  EXPECT_EQ(*copy[0], 1);
  EXPECT_EQ(*copy[1], 2);
}
