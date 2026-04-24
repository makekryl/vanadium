#pragma once

#include <algorithm>

#include "vanadium/runtime/rt_alloc.h"
#include "vanadium/runtime/rt_reflect.h"

namespace vanadium::rt::detail {

template <typename T>
struct ValueList {
  T* data;
  vrt_valuelist_size_t length;

  template <auto Matcher, typename V>
  bool MatchAny(const V* v) const {
    return std::any_of(data, data + length, [&v](const auto& e) {
      return Matcher(v, &e);
    });
  }

  template <auto Matcher, typename V>
  bool MatchAll(const V* v) const {
    return std::all_of(data, data + length, [&v](const auto& e) {
      return Matcher(v, &e);
    });
  }

  template <auto Destruct>
  void Release() {
    for (vrt_valuelist_size_t i = 0; i < length; ++i) {
      Destruct(&data[i]);
    }
    vrt_unifree(data);
  }
};

template <typename T>
struct Implication {
  T precondition;
  T implied;

  template <auto Matcher, typename V>
  bool Match(const V* v) const {
    return !Matcher(v, &precondition) || Matcher(v, &implied);
  }

  template <auto Destruct>
  void Release() {
    Destruct(&precondition);
    Destruct(&implied);
  }
};

inline bool DynamicMatch(const vrt_dynmatcher_t* dm, const void* obj) {
  return dm->match(dm->ctx, obj);
}

inline void FreeDynamicMatcher(const vrt_dynmatcher_t* p) {
  vrt_unifree(p->ctx);
}

}  // namespace vanadium::rt::detail
