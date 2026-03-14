#include "vanadium/runtime/rt_charstring.h"

#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/runtime.h"

// NOLINTBEGIN(readability-identifier-naming)

extern "C" {
void vrt_charstring_ctor(void*);
void vrt_charstring_dtor(void*);
}

const vrt_typeinfo_t charstring_typeinfo{
    .name = "charstring",
    .kind = vrt_typekind_e::kScalar,
    .bytes = sizeof(vrt_charstring_t),

    .members = nullptr,

    .construct = vrt_charstring_ctor,
    .destruct = vrt_charstring_dtor,
};

void vrt_charstring_ctor(void*) {
  vrt_panic("vrt_charstring_ctor should not be called");
}
void vrt_charstring_dtor(void* p) {
  // todo: owned strings
}

void copy_charstring(vrt_charstring_t* dst, vrt_charstring_t src) {
  dst->value = src.value;  // todo: copy when ownership is added
  dst->length = src.length;
  dst->is_bound = src.is_bound;
}

// NOLINTEND(readability-identifier-naming)
