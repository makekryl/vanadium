#pragma once

#include <algorithm>
#include <array>
#include <functional>
#include <utility>

namespace vanadium::lib {

template <typename K, typename V, std::size_t N>
class StaticMap final {
  using row_t = std::pair<K, V>;

 public:
  consteval StaticMap(std::array<row_t, N>&& data) : storage_(std::move(data)) {
    std::ranges::sort(storage_);
  }

  [[nodiscard]] std::optional<std::reference_wrapper<const V>> get(const K& key) const noexcept {
    if (auto it = find(key); it != storage_.end() && it->first == key) {
      return it->second;
    }
    return std::nullopt;
  }

  const V& operator[](const K& key) const noexcept {
    return *find_it(key);
  }

  auto Entries() const {
    return storage_;
  }

 private:
  auto find(const K& key) const noexcept {
    return std::ranges::lower_bound(storage_, key, std::ranges::less{}, &row_t::first);
  }

  std::array<row_t, N> storage_;
};

template <typename K, typename V, std::size_t N>
consteval auto MakeStaticMap(std::pair<K, V> (&&rows)[N]) {
  return StaticMap<K, V, N>(std::to_array(std::move(rows)));
}

}  // namespace vanadium::lib
