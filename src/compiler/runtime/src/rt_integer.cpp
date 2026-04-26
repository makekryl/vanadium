#include "vanadium/runtime/rt_integer.h"

#include <cassert>
#include <cstdio>

#include "vanadium/runtime/BuiltinsTemplates.h"
#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/TemplateMatching.h"
#include "vanadium/runtime/rt_alloc.h"
#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/rt_template.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

namespace {
inline void AssertIsBound(vrt_integer_t& i) {
  rt::Assert(i.is_bound, "accessing an unbound integer value");
}
}  // namespace

const vrt_typeinfo_t integer_typeinfo{
    .name = "integer",
    .kind = vrt_typekind_e::kInteger,
    .size = sizeof(vrt_integer_t),

    .members = nullptr,

    .construct = vanadium::rt::helpers::VoidErased<vrt_integer_ctor>,
    .destruct = vanadium::rt::helpers::VoidErased<vrt_integer_dtor>,

    .counterpart = &integer_template_typeinfo,
};

extern "C" {
void vrt_integer_dtor_big(vrt_integer_t* p) {
  // assert(p->is_bound && p->is_big);
  // TODO: implement big numbers support
}
}

void vrt_integer_ctor(vrt_integer_t* p) {
  p->is_bound = false;
  // p->is_big = false;
}
void vrt_integer_dtor(vrt_integer_t* p) {
  // if (p->is_bound && p->is_big) {
  //   vrt_integer_dtor_big(p);
  // }
}

#define DEFINE_BINARY_OP_TO_BOOL(name, op)                    \
  bool vrt_integer_##name(vrt_integer_t a, vrt_integer_t b) { \
    AssertIsBound(a);                                         \
    AssertIsBound(b);                                         \
    return a.value op b.value;                                \
  }
#define DEFINE_BINARY_OP_TO_VALUE(name, op)                            \
  vrt_integer_t vrt_integer_##name(vrt_integer_t a, vrt_integer_t b) { \
    AssertIsBound(a);                                                  \
    AssertIsBound(b);                                                  \
    return vrt_integer_wrap(a.value op b.value);                       \
  }

void copy_integer(vrt_integer_t* dst, vrt_integer_t src) {
  *dst = src;
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

vrt_integer_t vrt_integer_neg(vrt_integer_t a) {
  AssertIsBound(a);
  return vrt_integer_wrap(-a.value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" {
void vrt_integer_template_ctor(vrt_integer_template_t*);
void vrt_integer_template_dtor(vrt_integer_template_t*);
}

const vrt_typeinfo_t integer_template_typeinfo{
    .name = integer_typeinfo.name,
    .kind = integer_typeinfo.kind,
    .is_template = true,
    .size = sizeof(vrt_integer_template_t),

    .members = integer_typeinfo.members,

    .construct = vanadium::rt::helpers::VoidErased<vrt_integer_template_ctor>,
    .destruct = vanadium::rt::helpers::VoidErased<vrt_integer_template_dtor>,

    .counterpart = &integer_typeinfo,
};

void vrt_integer_template_ctor(vrt_integer_template_t* p) {
  rt::tpl::Construct(p);
}
void vrt_integer_template_dtor(vrt_integer_template_t* p) {
  switch (p->tsel) {
    case vrt_template_sel_e::kSpecificValue:
    case vrt_template_sel_e::kValueRange:
      break;
    default:
      rt::tpl::Destruct<vrt_integer_template_dtor>(p);
      break;
  }
}

bool vrt_integer_template_match(const vrt_integer_t* v, const vrt_integer_template_t* t) {
  switch (t->tsel) {
    case vrt_template_sel_e::kSpecificValue:
      return v->value == t->val.value;
    case vrt_template_sel_e::kValueRange: {
      bool matches = true;
      if (t->vmin_present) {
        matches = matches && (t->vmin_exclusive ? (t->range.vmin < v->value) : (t->range.vmin <= v->value));
      }
      if (t->vmax_exclusive) {
        matches = matches && (t->vmax_exclusive ? (t->range.vmax > v->value) : (t->range.vmax >= v->value));
      }
      return matches;
    }
    default:
      return rt::tpl::Match<vrt_integer_template_match>(v, t);
  }
}
