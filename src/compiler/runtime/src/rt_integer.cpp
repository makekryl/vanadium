#include "vanadium/runtime/rt_integer.h"

#include "vanadium/runtime/runtime.h"

namespace {
inline void CheckIsBound(vrt_int_t i) {
  if (!i.is_bound) [[unlikely]] {
    vrt_panic("UNBOUND");
  }
}
}  // namespace

const vrt_typeinfo_t integer_typeinfo{
    .name = "integer",
    .kind = vrt_typekind_e::kScalar,
    .bytes = sizeof(vrt_int_t),

    .members = nullptr,

    .construct = nullptr,
    .destruct = nullptr,
};

#define DEFINE_BINARY_OP_TO_BOOL(name, op)        \
  bool vrt_int_##name(vrt_int_t a, vrt_int_t b) { \
    CheckIsBound(a);                              \
    CheckIsBound(b);                              \
    return a.value op b.value;                    \
  }
#define DEFINE_BINARY_OP_TO_VALUE(name, op)            \
  vrt_int_t vrt_int_##name(vrt_int_t a, vrt_int_t b) { \
    CheckIsBound(a);                                   \
    CheckIsBound(b);                                   \
    return {                                           \
        .value = a.value op b.value,                   \
        .is_bound = true,                              \
        .is_big = false,                               \
    };                                                 \
  }

void copy_integer(vrt_int_t* dst, vrt_int_t src) {
  dst->value = src.value;
  dst->is_big = src.is_big;
  dst->is_bound = src.is_bound;
}

DEFINE_BINARY_OP_TO_BOOL(eq, ==);
DEFINE_BINARY_OP_TO_BOOL(ne, !=);
//
DEFINE_BINARY_OP_TO_BOOL(lt, <);
DEFINE_BINARY_OP_TO_BOOL(le, <=);
DEFINE_BINARY_OP_TO_BOOL(gt, >);
DEFINE_BINARY_OP_TO_BOOL(ge, >=);

DEFINE_BINARY_OP_TO_VALUE(add, +);
DEFINE_BINARY_OP_TO_VALUE(sub, -);
DEFINE_BINARY_OP_TO_VALUE(mul, *);
DEFINE_BINARY_OP_TO_VALUE(div, /);
