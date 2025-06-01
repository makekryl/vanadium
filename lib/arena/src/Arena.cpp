#include "Arena.h"

#include <algorithm>
#include <format>
#include <iostream>
#include <utility>

namespace {
inline std::byte* arena_allocate(std::size_t size) {
  return new std::byte[size];
}
inline void arena_deallocate(void* p) {
  delete[] reinterpret_cast<std::byte*>(p);
}
}  // namespace

namespace vanadium::lib {

Arena::Arena() : active_(nullptr), cleanup_list_(nullptr), bytes_allocated_(0), bytes_used_(0) {}

Arena::Arena(Arena&& other) noexcept
    : active_(std::exchange(other.active_, nullptr)),
      cleanup_list_(std::exchange(other.cleanup_list_, nullptr)),
      bytes_allocated_(std::exchange(other.bytes_allocated_, 0)),
      bytes_used_(std::exchange(other.bytes_used_, 0)) {}

Arena& Arena::operator=(Arena&& other) noexcept {
  using std::swap;
  swap(active_, other.active_);
  swap(cleanup_list_, other.cleanup_list_);
  swap(bytes_allocated_, other.bytes_allocated_);
  swap(bytes_used_, other.bytes_used_);
  return *this;
}

Arena::~Arena() {
  // std::cerr << std::format(" ~Arena({} / {} B)\n", bytes_used_, bytes_allocated_);
  Release();
}

inline Arena::Block::Block(std::size_t size) : pos(Begin()), end(pos + size) {}
inline std::byte* Arena::Block::Begin() const noexcept {
  return reinterpret_cast<std::byte*>(const_cast<Block*>(this)) + sizeof(*this);
}
inline std::size_t Arena::Block::Size() const noexcept {
  return end - Begin();
}

Arena::Block* Arena::AllocateNewBlock(std::size_t size) {
  auto* const p = arena_allocate(sizeof(Block) + size);
  auto* const block = new (p) Block(size);

  bytes_allocated_ += size;

  return block;
}

std::byte* Arena::AllocBuffer(std::size_t size, std::size_t alignment) {
  const auto get_remaining_space = [&] {
    return active_->end - active_->pos;
  };
  std::size_t space;

  if (active_ == nullptr) [[unlikely]] {
    active_ = AllocateNewBlock(std::max(size, 256UL));
    space = get_remaining_space();
  } else {
    space = get_remaining_space();
    if (space < (size + alignment - 1)) {
      auto* const new_block = AllocateNewBlock(std::max(size, 2 * active_->Size()));
      new_block->next = std::exchange(active_, new_block);
      space = get_remaining_space();
    }
  }

  void* ptr = active_->pos;
  auto* aligned_ptr = reinterpret_cast<std::byte*>(std::align(alignment, size, ptr, space));
  active_->pos = aligned_ptr + size;
  bytes_used_ += size;
  return aligned_ptr;
}

void Arena::AddToCleanupList(void* obj, CleanupFunc cleanup) {
  auto* node = Alloc<CleanupNode>();
  node->obj = obj;
  node->cleanup = cleanup;
  node->next = std::exchange(cleanup_list_, node);
}

void Arena::Reset() {
  if (active_ == nullptr) [[unlikely]] {
    return;
  }

  for (CleanupNode* item = cleanup_list_; item != nullptr; item = item->next) {
    item->cleanup(item->obj);
  }
  cleanup_list_ = nullptr;

  // Keep the first block for later reuse
  Block* b = active_->next;
  while (b) {
    auto* const next = b->next;
    arena_deallocate(b);
    b = next;
  }
  active_->pos = active_->Begin();
  active_->next = nullptr;

  bytes_allocated_ = active_->Size();
  bytes_used_ = 0;
}

void Arena::Release() {
  Reset();
  arena_deallocate(active_);
  active_ = nullptr;
}

};  // namespace vanadium::lib
