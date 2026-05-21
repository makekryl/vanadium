#include "vanadium/runtime/runtime.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <stdexcept>
#include <string>

#include "vanadium/runtime/Stringifier.h"
#include "vanadium/runtime/TemplateMatching.h"
#include "vanadium/runtime/TypeHelper.h"
#include "vanadium/runtime/UnionLayout.h"
#include "vanadium/runtime/rt_bitstring.h"
#include "vanadium/runtime/rt_boolean.h"
#include "vanadium/runtime/rt_charstring.h"
#include "vanadium/runtime/rt_float.h"
#include "vanadium/runtime/rt_hexstring.h"
#include "vanadium/runtime/rt_integer.h"
#include "vanadium/runtime/rt_octetstring.h"
#include "vanadium/runtime/rt_reflect.h"
#include "vanadium/runtime/rt_template.h"

using namespace vanadium;

void vrt_panic(const char* rr) {
  throw std::runtime_error(rr);
}

bool vrt_is_bound(const vrt_val_t* v) {
  if (rt::tpl::IsTemplateType(v->ty)) {
    return static_cast<const rt::tpl::GenericTemplateType*>(v->p)->tsel != vrt_template_sel_e::kUninitialized;
  }

#define X(name)                                                \
  if (v->ty == &name##_typeinfo) {                             \
    return static_cast<const vrt_##name##_t*>(v->p)->is_bound; \
  }
#include "vanadium/runtime/BuiltinTypes.inc"
#undef X

  switch (v->ty->kind) {
    case vrt_typekind_e::kRecord:
    case vrt_typekind_e::kSet:
      return std::ranges::any_of(std::span{v->ty->members, v->ty->members_count}, [&](const auto& m) {
        const vrt_val_t mv{.p = rt::GetMemberPtr(m, v->p), .ty = m.type};
        return vrt_is_bound(&mv);
      });
    case vrt_typekind_e::kUnion:
      return reinterpret_cast<const rt::GenericUnion*>(v->p)->active_idx != rt::GenericUnion::kUnboundActiveIdx;
    default:
      assert(false);
      return false;
  }
}

void vrt_log(const vrt_val_t* args, std::uint32_t n) {
  static thread_local std::string buf;
  buf.clear();

  std::for_each_n(args, n, [&](const auto& arg) {
    rt::StringifyObject(buf, arg);
    buf += " ";
  });
  vrt_log_write(buf.c_str());
}
