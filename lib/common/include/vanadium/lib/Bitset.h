#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>

#include "vanadium/lib/HeapArray.h"
#include "vanadium/lib/ManualLifetime.h"

namespace vanadium::lib {

class Bitset {
 public:
  using storage_t = std::uint64_t;

  static constexpr std::size_t kCellBits = std::numeric_limits<storage_t>::digits;

  Bitset(std::size_t bits = 0) : bits_(bits) {
    if (UsesHeap()) {
      storage_.heap.construct(GetCellCount(bits_));
    } else {
      storage_.small.fill(0);
    }
  }

  Bitset(const Bitset& other) : bits_(other.bits_) {
    if (UsesHeap()) {
      storage_.heap.construct(*other.storage_.heap);
    } else {
      storage_.small = other.storage_.small;
    }
  }

  Bitset(Bitset&& other) noexcept : bits_(other.bits_) {
    if (UsesHeap()) {
      storage_.heap.construct(std::move(*other.storage_.heap));
      other.storage_.heap.destroy();
    } else {
      storage_.small = std::move(other.storage_.small);
    }

    other.bits_ = 0;
  }

  Bitset& operator=(const Bitset& other) {
    if (this != &other) {
      Bitset tmp(other);
      *this = std::move(tmp);
    }
    return *this;
  }

  Bitset& operator=(Bitset&& other) noexcept {
    if (this != &other) {
      if (other.UsesHeap()) {
        if (UsesHeap()) {
          *storage_.heap = std::move(*other.storage_.heap);
        } else {
          storage_.heap.construct(std::move(*other.storage_.heap));
        }
        other.storage_.heap.destroy();
      } else {
        if (UsesHeap()) {
          storage_.heap.destroy();
        }
        storage_.small = std::move(other.storage_.small);
      }
      bits_ = std::exchange(other.bits_, 0);
    }
    return *this;
  }

  ~Bitset() {
    if (UsesHeap()) {
      storage_.heap.destroy();
    }
  }

  [[nodiscard]] bool Get(std::size_t pos) const {
    return (GetCell(pos / kCellBits) & GetMask(pos)) != 0;
  }

  void Set(std::size_t pos) {
    GetCell(pos / kCellBits) |= GetMask(pos);
  }

  void Clear(std::size_t pos) {
    GetCell(pos / kCellBits) &= ~GetMask(pos);
  }

  void operator&=(const Bitset& other) {
    assert(bits_ == other.bits_);

    for (std::size_t i = 0; i < CellCount(); ++i) {
      GetCell(i) &= other.GetCell(i);
    }
  }

  void operator|=(const Bitset& other) {
    assert(bits_ == other.bits_);

    for (std::size_t i = 0; i < CellCount(); ++i) {
      GetCell(i) |= other.GetCell(i);
    }
  }

  void Flip() {
    for (std::size_t i = 0; i < CellCount(); ++i) {
      GetCell(i) = ~GetCell(i);
    }

    if (bits_ % kCellBits != 0) {
      GetCell(CellCount() - 1) &= (storage_t{1} << (bits_ % kCellBits)) - 1;
    }
  }

  void Reset() {
    if (UsesHeap()) {
      storage_.heap->fill(0);
    } else {
      storage_.small.fill(0);
    }
  }

  [[nodiscard]] std::size_t ActiveCount() const {
    std::size_t count = 0;

    for (std::size_t i = 0; i < CellCount(); ++i) {
      count += std::popcount(GetCell(i));
    }

    return count;
  }

  [[nodiscard]] std::size_t InactiveCount() const {
    return bits_ - ActiveCount();
  }

  [[nodiscard]] bool All() const {
    return bits_ == ActiveCount();
  }

  [[nodiscard]] std::size_t Size() const {
    return bits_;
  }

 private:
  [[nodiscard]] static constexpr std::size_t GetCellCount(std::size_t bits) {
    return (bits / kCellBits) + (bits % kCellBits != 0 ? 1 : 0);
  }

  [[nodiscard]] bool UsesHeap() const {
    return GetCellCount(bits_) > storage_.small.max_size();
  }

  [[nodiscard]] std::size_t CellCount() const {
    return GetCellCount(bits_);
  }

  [[nodiscard]] storage_t& GetCell(std::size_t idx) {
    return const_cast<storage_t&>(std::as_const(*this).GetCell(idx));
  }

  [[nodiscard]] const storage_t& GetCell(std::size_t idx) const {
    return UsesHeap() ? (*storage_.heap)[idx] : storage_.small[idx];
  }

  [[nodiscard]] constexpr storage_t GetMask(std::size_t pos) const {
    assert(pos < bits_ && "Bitset index is out of bounds");
    return storage_t{1} << (pos % kCellBits);
  }

  std::size_t bits_;
  union Storage {
    ManualLifetime<HeapArray<storage_t>> heap;
    std::array<storage_t, sizeof(heap) / sizeof(storage_t)> small;
  } storage_;
};

}  // namespace vanadium::lib
