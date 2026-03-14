#include "vanadium/runtime/runtime.h"

#include <cstdint>
#include <cstdio>
#include <print>
#include <stdexcept>
#include <string>

#include "vanadium/runtime/RuntimeInternals.h"
#include "vanadium/runtime/rt_charstring.h"
#include "vanadium/runtime/rt_integer.h"
#include "vanadium/runtime/rt_reflect.h"

void vrt_panic(const char* rr) {
  throw std::runtime_error(rr);
}

extern "C" {
bool vrt_is_bound(const vrt_val_t* v) {
  if (v->ty == &integer_typeinfo) {
    return static_cast<const vrt_int_t*>(v->p)->is_bound;
  }
  if (v->ty == &charstring_typeinfo) {
    return static_cast<const vrt_charstring_t*>(v->p)->is_bound;
  }
  return true;
}

void vrt_log(const vrt_val_t* args, std::uint32_t n) {
  std::string buf;
  for (const auto* arg = args; arg < args + n; ++arg) {
    if (!vrt_is_bound(arg)) {
      buf += "<unbound>";
    } else if (arg->ty == &integer_typeinfo) {
      buf += std::to_string(static_cast<const vrt_int_t*>(arg->p)->value);
    } else if (arg->ty == &charstring_typeinfo) {
      buf += "\"";
      buf += static_cast<const vrt_charstring_t*>(arg->p)->value;
      buf += "\"";
    } else {
      buf += "<unknown_typeinfo>";
    }
    buf += " ";
  }
  std::println("LOG :: {}", buf);
  std::fflush(stdout);
}
}
