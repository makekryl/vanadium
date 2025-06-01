#pragma once

#include <cstddef>
#include <list>
#include <type_traits>
#include <unordered_map>

namespace vanadium::lib {

template <typename TKey, typename TValue, std::size_t Capacity>
  requires(std::is_trivially_destructible_v<TKey> && std::is_trivially_destructible_v<TValue>)
class LruCache {
 public:
  using entry_t = std::pair<TKey, TValue>;
  using list_iterator_t = std::list<entry_t>::iterator;

  void put(const TKey& key, const TValue& value) {
    auto it = item_map_.find(key);
    item_list_.push_front(entry_t(key, value));
    if (it != item_map_.end()) {
      item_list_.erase(it->second);
      item_map_.erase(it);
    }
    item_map_[key] = item_list_.begin();

    if (item_map_.size() > Capacity) {
      auto last = item_list_.end();
      last--;
      item_map_.erase(last->first);
      item_list_.pop_back();
    }
  }

  [[nodiscard]] const TValue* get(const TKey& key) {
    const auto it = item_map_.find(key);
    if (it == item_map_.end()) {
      return nullptr;
    }
    item_list_.splice(item_list_.begin(), item_list_, it->second);
    return &it->second->second;
  }

  [[nodiscard]] std::size_t size() const {
    return item_map_.size();
  }

  void reset() {
    item_map_.clear();
    item_list_.clear();
  }

 private:
  std::list<entry_t> item_list_;
  std::unordered_map<TKey, list_iterator_t> item_map_;
};

}  // namespace vanadium::lib
