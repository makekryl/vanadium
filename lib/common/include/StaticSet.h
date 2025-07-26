#pragma once

#include <algorithm>
#include <array>
#include <utility>

namespace vanadium::lib {

template <typename E, std::size_t N>
class StaticSet final {
 public:
  consteval StaticSet(std::array<E, N>&& data) : storage_(std::move(data)) {
    std::ranges::sort(storage_);
  }

  [[nodiscard]] bool contains(const E& element) const noexcept {
    const auto it = find(element);
    return it != storage_.end() && *it == element;
  }

 private:
  auto find(const E& element) const noexcept {
    return std::ranges::lower_bound(storage_, element);
  }

  std::array<E, N> storage_;
};

template <typename E, std::size_t N>
consteval auto MakeStaticSet(E (&&records)[N]) {
  return StaticSet<E, N>(std::to_array(std::move(records)));
}

}  // namespace vanadium::lib
