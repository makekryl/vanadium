#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>

#include "vanadium/lib/HeapArray.h"

namespace vanadium::lib {

class Bitset {
 public:
  using storage_t = std::uint64_t;
  static constexpr std::size_t kCellBits = std::numeric_limits<storage_t>::digits;

  Bitset(std::size_t bits = 0) : bits_(bits), cells_(bits ? (((bits - 1) / kCellBits) + 1) : 0) {}

  [[nodiscard]] bool Get(std::size_t pos) const {
    CheckBounds(pos);
    return (cells_[GetCellIndex(pos)] & GetMask(pos)) != 0;
  }

  void Set(std::size_t pos) {
    CheckBounds(pos);
    cells_[GetCellIndex(pos)] |= GetMask(pos);
  }

  void Clear(std::size_t pos) {
    CheckBounds(pos);
    cells_[GetCellIndex(pos)] &= ~GetMask(pos);
  }

  void operator&=(const Bitset& other) {
    assert(bits_ == other.bits_ && "Cannot perform &= on bitsets of different sizes");
    for (std::size_t i = 0; i < cells_.Size(); i++) {
      cells_[i] &= other.cells_[i];
    }
  }

  void operator|=(const Bitset& other) {
    assert(bits_ == other.bits_ && "Cannot perform &= on bitsets of different sizes");
    for (std::size_t i = 0; i < cells_.Size(); i++) {
      cells_[i] |= other.cells_[i];
    }
  }

  void Flip() {
    for (std::size_t i = 0; i < cells_.Size(); i++) {
      cells_[i] = ~cells_[i];
    }
  }

  void Reset() {
    for (std::size_t i = 0; i < cells_.Size(); i++) {
      cells_[i] = 0;
    }
  }

  [[nodiscard]] std::size_t ActiveCount() const {
    std::size_t count{0};
    for (std::size_t i = 0; i < cells_.Size(); i++) {
      count += std::popcount(cells_[i]);
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
  [[nodiscard]] static std::size_t GetCellIndex(std::size_t pos) {
    return pos / kCellBits;
  }
  [[nodiscard]] static storage_t GetMask(std::size_t pos) {
    return storage_t{storage_t(1) << (pos % kCellBits)};
  }

  void CheckBounds(std::size_t pos) const {
    (void)pos;
    assert(pos < bits_ && "Bitset index is out of bounds");
  }

  std::size_t bits_;
  HeapArray<storage_t> cells_;
};

}  // namespace vanadium::lib
