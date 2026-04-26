#include "vanadium/runtime/rt_boolean.h"

#include <cassert>

#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/TemplateMatching.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

namespace {
inline void CheckIsBound(vrt_boolean_t b) {
  rt::Assert(b.is_bound, "accessing an unbound boolean value");
}
}  // namespace

const vrt_typeinfo_t boolean_typeinfo{
    .name = "boolean",
    .kind = vrt_typekind_e::kBoolean,
    .size = sizeof(vrt_boolean_t),

    .members = nullptr,

    .construct = nullptr,
    .destruct = nullptr,
};

void copy_boolean(vrt_boolean_t* dst, vrt_boolean_t src) {
  *dst = src;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
void vrt_boolean_template_ctor(vrt_boolean_template_t*);
void vrt_boolean_template_dtor(vrt_boolean_template_t*);
}

struct vrt_boolean_template_t {
  vrt_template_sel_e tsel;

  //
  // moved out of the inner struct to avoid extra padding, saving up 8 bytes
  bool vmin_exclusive;
  bool vmax_exclusive;
  bool vmin_present;
  bool vmax_present;
  //

  union {
    vrt_boolean_t val;
    rt::tpl::ValueList<vrt_boolean_template_t> list;
    rt::tpl::Implication<vrt_boolean_template_t>* implication;
    vrt_dynmatcher_t dynmatch;
  };
};

extern "C" {
void vrt_boolean_template_ctor(vrt_boolean_template_t*);
void vrt_boolean_template_dtor(vrt_boolean_template_t*);
}

const vrt_typeinfo_t boolean_template_typeinfo{
    .name = boolean_typeinfo.name,
    .kind = boolean_typeinfo.kind,
    .is_template = true,
    .size = sizeof(vrt_boolean_template_t),

    .members = boolean_typeinfo.members,

    .construct = vanadium::rt::helpers::VoidErased<vrt_boolean_template_ctor>,
    .destruct = vanadium::rt::helpers::VoidErased<vrt_boolean_template_dtor>,

    .counterpart = &boolean_typeinfo,
};

void vrt_boolean_template_ctor(vrt_boolean_template_t* p) {
  rt::tpl::Construct(p);
}
void vrt_boolean_template_dtor(vrt_boolean_template_t* p) {
  rt::tpl::Destruct<vrt_boolean_template_dtor>(p);
}

bool vrt_boolean_template_match(const vrt_boolean_t* v, const vrt_boolean_template_t* t) {
  switch (t->tsel) {
    case vrt_template_sel_e::kSpecificValue:
      return v->value == t->val.value;
    default:
      return rt::tpl::Match<vrt_boolean_template_match>(v, t);
  }
}
