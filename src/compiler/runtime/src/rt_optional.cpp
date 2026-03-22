#include "vanadium/runtime/rt_optional.h"

#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/runtime.h"

// NOLINTBEGIN(readability-identifier-naming)

extern "C" {
void vrt_optional_ctor(void*);
void vrt_optional_dtor(void*);
}

const vrt_typeinfo_t vrt_optional_typeinfo{
    .name = "vrt_optional",
    .kind = vrt_typekind_e::kScalar,
    .bytes = sizeof(vrt_optional_t),

    .members = nullptr,

    .construct = vrt_optional_ctor,
    .destruct = vrt_optional_dtor,
};

void vrt_optional_ctor(void*) {
  vrt_panic("vrt_optional_ctor should not be called");
}
void vrt_optional_dtor(void* p) {
  auto* o = reinterpret_cast<vrt_optional_t*>(p);
  if (o->value) {
    vrt_del(&vrt_optional_typeinfo, o->value);
  }
}

void* vrt_optional_get(const vrt_optional_t* o) {
  return o->value;
}
void vrt_optional_set(vrt_optional_t* o, void* v) {
  if (o->value) {
    vrt_del(o->member_type, o->value);
  }
  o->value = v;
}

bool vrt_optional_is_present(const vrt_optional_t* o) {
  return o->value != nullptr;
}

// NOLINTEND(readability-identifier-naming)
