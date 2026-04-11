#include "vanadium/runtime/runtime.h"

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <format>
#include <limits>
#include <print>
#include <stdexcept>
#include <string>

#include "vanadium/runtime/rt_bitstring.h"
#include "vanadium/runtime/rt_boolean.h"
#include "vanadium/runtime/rt_charstring.h"
#include "vanadium/runtime/rt_float.h"
#include "vanadium/runtime/rt_integer.h"
#include "vanadium/runtime/rt_octetstring.h"
#include "vanadium/runtime/rt_reflect.h"

namespace {
constexpr auto kHexDigits =
    std::to_array({'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'});
}

void vrt_panic(const char* rr) {
  throw std::runtime_error(rr);
}

extern "C" {
bool vrt_is_bound(const vrt_val_t* v) {
  if (v->ty == &integer_typeinfo) {
    return static_cast<vrt_int_t*>(v->p)->is_bound;
  }
  if (v->ty == &charstring_typeinfo) {
    return static_cast<vrt_charstring_t*>(v->p)->is_bound;
  }
  return true;
}

void vrt_log(const vrt_val_t* args, std::uint32_t n) {
  std::string buf;
  for (const auto* arg = args; arg < args + n; ++arg) {
    // if (arg->ty == &charstring_typeinfo) {
    //   auto* s = static_cast<vrt_charstring_t*>(arg->p);
    //   std::println("CS({:p}): len={}, ext.ptr={:p} ext={}, bound={}", (void*)s, s->length, (void*)s->value.ext.data,
    //                s->is_ext, s->is_bound);
    // }
    if (!vrt_is_bound(arg)) {
      buf += "<unbound>";
    } else if (arg->ty == &integer_typeinfo) {
      buf += std::to_string(static_cast<vrt_int_t*>(arg->p)->value);
    } else if (arg->ty == &float_typeinfo) {
      const auto val = static_cast<vrt_float_t*>(arg->p)->value;
      constexpr auto kInfty = std::numeric_limits<decltype(val)>::infinity();
      if (val == kInfty) {
        buf += "infinity";
      } else if (val == -kInfty) {
        buf += "-infinity";
      } else if (std::isnan(val)) {
        buf += "not_a_number";
      } else if (val == 0.0) {
        buf += "0.0";
      } else {
        const double abs_val = std::abs(val);
        const double exponent = std::floor(std::log10(abs_val));
        const double mantissa = abs_val / std::pow(10.0, exponent);
        if (val < 0.0) {
          buf += "-";
        }
        std::format_to(std::back_inserter(buf), "{:.15g}", mantissa);
        if (std::floor(mantissa) == mantissa) {
          buf += ".0";
        }
        if (exponent != 0.0) {
          std::format_to(std::back_inserter(buf), "e{}", static_cast<int>(exponent));
        }
      }
    } else if (arg->ty == &boolean_typeinfo) {
      buf += static_cast<vrt_bool_t*>(arg->p)->value ? "true" : "false";
    } else if (arg->ty == &charstring_typeinfo) {
      auto* s = static_cast<vrt_charstring_t*>(arg->p);
      buf += "\"";
      buf += std::string_view{vrt_charstring_get_cbuf(s), s->length};
      buf += "\"";
    } else if (arg->ty == &octetstring_typeinfo) {
      auto* s = static_cast<vrt_octetstring_t*>(arg->p);
      const auto* sdata = vrt_octetstring_get_cbuf(s);
      buf += "'";
      for (std::uint32_t i = 0; i < s->length; ++i) {
        const auto& octet = sdata[i];
        buf += kHexDigits[octet >> 4];
        buf += kHexDigits[octet & 0x0F];
      }
      buf += "'O";
    } else if (arg->ty == &bitstring_typeinfo) {
      auto* s = static_cast<vrt_bitstring_t*>(arg->p);
      const auto* sdata = vrt_bitstring_get_buf(s);
      buf += "'";
      //
      const auto full_bytes = s->length / 8;
      const auto remaining = s->length % 8;
      //
      for (size_t i = 0; i < full_bytes; ++i) {
        const std::uint8_t byte = sdata[i];
        for (int b = 7; b >= 0; --b) {
          buf += ((byte >> b) & 1) ? '1' : '0';
        }
      }
      if (remaining) {
        const std::uint8_t byte = sdata[full_bytes];
        for (int b = 0; b < remaining; ++b) {
          buf += ((byte >> (7 - b)) & 1) ? '1' : '0';
        }
      }
      //
      buf += "'B";
    } else {
      buf += "<unknown_typeinfo>";
    }
    buf += " ";
  }
  std::println("LOG :: {}", buf);
  std::fflush(stdout);
}
}
