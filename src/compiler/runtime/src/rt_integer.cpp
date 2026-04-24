#include "vanadium/runtime/rt_integer.h"

#include <algorithm>
#include <cassert>

#include "vanadium/runtime/RuntimeHelpers.h"
#include "vanadium/runtime/TemplateMatching.h"
#include "vanadium/runtime/rt_alloc.h"
#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/rt_template.h"
#include "vanadium/runtime/runtime.h"
#include "vanadium/runtime/runtime.hpp"

namespace {
inline void AssertIsBound(vrt_int_t& i) {
  rt::Assert(i.is_bound, "accessing an unbound integer value");
}
}  // namespace

const vrt_typeinfo_t integer_typeinfo{
    .name = "integer",
    .kind = vrt_typekind_e::kScalar,
    .size = sizeof(vrt_int_t),

    .members = nullptr,

    .construct = vanadium::rt::helpers::VoidErased<vrt_int_ctor>,
    .destruct = vanadium::rt::helpers::VoidErased<vrt_int_dtor>,

    .counterpart = &integer_template_typeinfo,
};

extern "C" {
void vrt_int_dtor_big(vrt_int_t* p) {
  // assert(p->is_bound && p->is_big);
  // TODO: implement big numbers support
}
}

void vrt_int_ctor(vrt_int_t* p) {
  p->is_bound = false;
  // p->is_big = false;
}
void vrt_int_dtor(vrt_int_t* p) {
  // if (p->is_bound && p->is_big) {
  //   vrt_int_dtor_big(p);
  // }
}

#define DEFINE_BINARY_OP_TO_BOOL(name, op)        \
  bool vrt_int_##name(vrt_int_t a, vrt_int_t b) { \
    AssertIsBound(a);                             \
    AssertIsBound(b);                             \
    return a.value op b.value;                    \
  }
#define DEFINE_BINARY_OP_TO_VALUE(name, op)            \
  vrt_int_t vrt_int_##name(vrt_int_t a, vrt_int_t b) { \
    AssertIsBound(a);                                  \
    AssertIsBound(b);                                  \
    return vrt_int_wrap(a.value op b.value);           \
  }

void copy_integer(vrt_int_t* dst, vrt_int_t src) {
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

vrt_int_t vrt_int_neg(vrt_int_t a) {
  AssertIsBound(a);
  return vrt_int_wrap(-a.value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct vrt_int_template_implication_t;
struct vrt_int_template_t {
  vrt_template_sel_e tsel;

  //
  // moved out of the inner struct to avoid extra padding, saving up 8 bytes
  bool vmin_exclusive;
  bool vmax_exclusive;
  bool vmin_present;
  bool vmax_present;
  //

  union {
    vrt_native_int_t val;
    struct {
      vrt_int_template_t* data;
      vrt_valuelist_size_t length;
    } list;
    struct {
      vrt_native_int_t vmin;
      vrt_native_int_t vmax;
    } range;
    vrt_int_template_implication_t* implication;
    vrt_dynmatcher_t dynmatch;
  };
};

struct vrt_int_template_implication_t {
  vrt_int_template_t precondition;
  vrt_int_template_t implied;
};

extern "C" {
void vrt_int_template_ctor(vrt_int_template_t*);
void vrt_int_template_dtor(vrt_int_template_t*);
}

const vrt_typeinfo_t integer_template_typeinfo{
    .name = "integer",
    .kind = vrt_typekind_e::kScalar,
    .is_template = true,
    .size = sizeof(vrt_int_template_t),

    .members = integer_typeinfo.members,

    .construct = vanadium::rt::helpers::VoidErased<vrt_int_template_ctor>,
    .destruct = vanadium::rt::helpers::VoidErased<vrt_int_template_dtor>,

    .counterpart = &integer_typeinfo,
};

void vrt_int_template_ctor(vrt_int_template_t* p) {}
void vrt_int_template_dtor(vrt_int_template_t* p) {
  switch (p->tsel) {
    case vrt_template_sel_e::kSpecificValue:
    case vrt_template_sel_e::kOmitValue:
    case vrt_template_sel_e::kAnyValue:
    case vrt_template_sel_e::kAnyOrOmit:
    case vrt_template_sel_e::kValueRange:
      break;
    case vrt_template_sel_e::kValueList:
    case vrt_template_sel_e::kComplementedList:
    case vrt_template_sel_e::kConjunctionMatch:
      vrt_unifree(p->list.data);
      break;
    case vrt_template_sel_e::kImplicationMatch:
      vrt_unifree(p->implication);
      break;
    case vrt_template_sel_e::kDynamicMatch:
      vrt_dynmatcher_free(&p->dynmatch);
      break;
    default:
      assert(false);
  }
}

bool vrt_int_template_match(const vrt_int_t* v, const vrt_int_template_t* t) {
  switch (t->tsel) {
    case vrt_template_sel_e::kSpecificValue:
      return v->value == t->val;

    case vrt_template_sel_e::kOmitValue:
      return false;

    case vrt_template_sel_e::kAnyValue:
    case vrt_template_sel_e::kAnyOrOmit:
      return true;

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

    case vrt_template_sel_e::kValueList:
    case vrt_template_sel_e::kComplementedList:
      return std::any_of(t->list.data, t->list.data + t->list.length,
                         [&v](const auto& e) {
                           return vrt_int_template_match(v, &e);
                         }) &&
             (t->tsel == vrt_template_sel_e::kValueList);

    case vrt_template_sel_e::kConjunctionMatch:
      return std::all_of(t->list.data, t->list.data + t->list.length, [&v](const auto& e) {
        return vrt_int_template_match(v, &e);
      });

    case vrt_template_sel_e::kImplicationMatch:
      return !vrt_int_template_match(v, &t->implication->precondition) ||
             vrt_int_template_match(v, &t->implication->implied);

    case vrt_template_sel_e::kDynamicMatch:
      return vanadium::rt::detail::InvokeDynamicMatcher(&t->dynmatch, v);

    default:
      assert(false);
      return false;
  }
}
