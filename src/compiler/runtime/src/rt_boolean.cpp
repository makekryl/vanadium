#include "vanadium/runtime/rt_boolean.h"

#include "vanadium/runtime/runtime.h"

namespace {
inline void CheckIsBound(vrt_int_t i) {
  if (!i.is_bound) [[unlikely]] {
    vrt_panic("UNBOUND");
  }
}
}  // namespace

const vrt_typeinfo_t boolean_typeinfo{
    .name = "boolean",
    .kind = vrt_typekind_e::kScalar,
    .bytes = sizeof(vrt_bool_t),

    .members = nullptr,

    .construct = nullptr,
    .destruct = nullptr,
};

void copy_boolean(vrt_bool_t* dst, vrt_bool_t src) {
  *dst = src;
}
