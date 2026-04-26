#include "vanadium/runtime/rt_reflect.h"

#include "vanadium/runtime/rt_alloc.h"

bool vrt_dynmatcher_invoke(const vrt_dynmatcher_t* dm, const void* obj) {
  return dm->match(dm->ctx, obj);
}

void vrt_dynmatcher_free(const vrt_dynmatcher_t* p) {
  vrt_unifree(p->ctx);
}
