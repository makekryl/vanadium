#include <gtest/gtest.h>

#include <cstddef>
#include <type_traits>

#include "Arena.h"

using namespace ::testing;

template <typename OnConstruct, typename OnDestruct = std::nullptr_t>
class HookedLifecycle {
 public:
  HookedLifecycle(OnConstruct on_construct, OnDestruct on_destruct = nullptr) : on_destruct_(std::move(on_destruct)) {
    on_construct();
  }
  ~HookedLifecycle() = default;
  ~HookedLifecycle()
    requires(!std::is_same_v<OnDestruct, std::nullptr_t>)
  {
    on_destruct_();
  }

 private:
  OnDestruct on_destruct_;
};

// TODO: consider hooking malloc/free for testing

TEST(ArenaTest, CallsConstructors) {
  constexpr std::size_t kObjects = 1024;
  std::size_t constructed_objects{0};

  const auto on_construct = [&]() {
    ++constructed_objects;
  };
  using TestObject = HookedLifecycle<decltype(on_construct)>;

  vanadium::lib::Arena arena;
  for (std::size_t i = 0; i < kObjects; i++) {
    arena.Alloc<TestObject>(on_construct);
  }
  EXPECT_EQ(constructed_objects, kObjects);
}

TEST(ArenaTest, CallsNonTrivialDestructors) {
  constexpr std::size_t kObjects = 1024;
  std::size_t constructed_objects{0};
  std::size_t destructed_objects{0};

  const auto on_construct = [&]() {
    ++constructed_objects;
  };
  const auto on_destruct = [&]() {
    ++destructed_objects;
  };
  using TestObject = HookedLifecycle<decltype(on_construct), decltype(on_destruct)>;

  vanadium::lib::Arena arena;
  for (std::size_t i = 0; i < kObjects; i++) {
    arena.Alloc<TestObject>(on_construct, on_destruct);
  }
  ASSERT_EQ(constructed_objects, kObjects);
  ASSERT_EQ(destructed_objects, 0);

  arena.Reset();
  EXPECT_EQ(destructed_objects, constructed_objects);
}
