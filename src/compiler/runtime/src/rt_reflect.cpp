#include "vanadium/runtime/rt_reflect.h"

#include "vanadium/runtime/TemplateMatching.h"

namespace rt = vanadium::rt;

bool vrt_dynmatcher_invoke(const vrt_dynmatcher_t* dm, const void* obj) {
  return rt::detail::DynamicMatch(dm, obj);
}

void vrt_dynmatcher_free(const vrt_dynmatcher_t* p) {
  rt::detail::FreeDynamicMatcher(p);
}
