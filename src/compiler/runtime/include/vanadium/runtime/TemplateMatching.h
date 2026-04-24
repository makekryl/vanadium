#pragma once

#include "vanadium/runtime/rt_alloc.h"
#include "vanadium/runtime/rt_reflect.h"

namespace vanadium::rt::detail {

inline bool InvokeDynamicMatcher(const vrt_dynmatcher_t* dm, const void* obj) {
  return dm->match(dm->ctx, obj);
}

inline void FreeDynamicMatcher(const vrt_dynmatcher_t* p) {
  vrt_unifree(p->ctx);
}

}  // namespace vanadium::rt::detail
