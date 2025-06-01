#pragma once

#include <algorithm>
#include <array>
#include <utility>

namespace vanadium::lib {

template <typename K, typename V, std::size_t N>
class StaticMap final {
 public:
  consteval StaticMap(std::array<std::pair<K, V>, N>&& data) : storage_(std::move(data)) {
    std::ranges::sort(storage_, &std::pair<K, V>::second);
  }

  auto find(const K& key) const noexcept {
    return find_it(key);
  }

  auto end() const noexcept {
    return storage_.end();
  }

  const V& operator[](const K& key) const noexcept {
    return *find_it(key);
  }

 private:
  auto find_it(const K& key) const noexcept {
    return std::ranges::lower_bound(storage_, key);
  }

  const std::array<std::pair<K, V>, N> storage_;
};

}  // namespace vanadium::lib
