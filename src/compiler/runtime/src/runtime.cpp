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
  if (v->ty->is_template) {
    return static_cast<const rt::tpl::GenericTemplateTypeLayout*>(v->p)->tsel != vrt_template_sel_e::kUninitialized;
  }

#define X(name)                                                \
  if (v->ty == &name##_typeinfo) {                             \
    return static_cast<const vrt_##name##_t*>(v->p)->is_bound; \
  }
#include "rt_builtin_types.inc"
#undef X

  assert(false);
  return false;
}

void vrt_log(const vrt_val_t* args, std::uint32_t n) {
  std::string buf;
  std::for_each_n(args, n, [&](const auto& arg) {
    printf("%p %s\n", arg.p, arg.ty->name);
    rt::StringifyObject(buf, arg);
    buf += " ";
  });
  vrt_log_write(buf.c_str());
}
