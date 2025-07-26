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
    return std::ranges::lower_bound(storage_, element) != storage_.end();
  }

 private:
  std::array<E, N> storage_;
};

template <typename E, typename... Ts>
constexpr StaticSet<E, sizeof...(Ts)> MakeStaticSet(Ts&&... t) {
  return {std::forward<Ts>(t)...};
}

}  // namespace vanadium::lib
