#pragma once

#include <cstddef>
#include <memory>
#include <span>
#include <type_traits>

namespace vanadium::lib {

class Arena {
 public:
  Arena() = default;

  Arena(Arena&&) noexcept;
  Arena& operator=(Arena&&) noexcept;

  Arena(const Arena&) = delete;
  Arena& operator=(const Arena&) = delete;

  ~Arena();

  __attribute__((malloc)) std::byte* AllocBuffer(std::size_t size, std::size_t alignment = alignof(std::max_align_t));

  template <typename T, typename... Args>
  __attribute__((malloc)) T* Alloc(Args&&... args) {
    auto* const p = AllocBuffer(sizeof(T), alignof(T));
    auto* const obj = new (p) T(std::forward<Args>(args)...);
    if constexpr (!std::is_trivially_destructible_v<T>) {
      AddToCleanupList(obj, arena_destruct_object<T>);
    }
    return obj;
  }

  std::span<char> AllocStringBuffer(std::size_t length) {
    auto* const buf = reinterpret_cast<char*>(AllocBuffer(length + 1, alignof(char)));
    buf[length] = 0;
    return {buf, length};
  }

  void Reset();
  void Release();

  [[nodiscard]] std::size_t SpaceAllocated() const noexcept {
    return bytes_allocated_;
  }
  [[nodiscard]] std::size_t SpaceUsed() const noexcept {
    return bytes_used_;
  }

 private:
  // Block's and CleanupNode's next is prev actually

  struct Block {
    std::byte* pos;
    std::byte* end;
    //
    Block* next{nullptr};

    Block(std::size_t size);

    [[nodiscard]] std::byte* Begin() const noexcept;
    [[nodiscard]] std::size_t Size() const noexcept;
  };

  using CleanupFunc = void (*)(void*);
  struct CleanupNode {
    void* obj;
    CleanupFunc cleanup;
    CleanupNode* next{nullptr};
  };

  template <typename T>
  static void arena_destruct_object(void* obj) {
    std::destroy_at(reinterpret_cast<T*>(obj));
  }

  void AddToCleanupList(void* obj, CleanupFunc cleanup);

 public:
  static constexpr std::size_t kBlockOverhead = sizeof(Block);

 private:
  Block* AllocateNewBlock(std::size_t size);

  Block* active_{nullptr};

  CleanupNode* cleanup_list_{nullptr};

  std::size_t bytes_allocated_{0};
  std::size_t bytes_used_{0};
};

};  // namespace vanadium::lib
