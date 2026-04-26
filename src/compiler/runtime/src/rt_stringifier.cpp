#include <cmath>
#include <format>
#include <limits>
#include <string>

#include "vanadium/runtime/Stringifier.h"
#include "vanadium/runtime/rt_integer.h"
#include "vanadium/runtime/runtime.h"

namespace vanadium::rt {

namespace {
constexpr auto kHexDigits = "0123456789ABCDEF";
}

void StringifyValue(std::string& buf, const vrt_val_t& v) {
  if (!vrt_is_bound(&v)) {
    buf += v.ty->is_template ? "<uninitialized template>" : "<unbound>";
  } else if (v.ty == &integer_typeinfo) {
    buf += std::to_string(static_cast<vrt_integer_t*>(v.p)->value);
  } else if (v.ty == &float_typeinfo) {
    const auto val = static_cast<vrt_float_t*>(v.p)->value;
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
  } else if (v.ty == &boolean_typeinfo) {
    buf += static_cast<vrt_boolean_t*>(v.p)->value ? "true" : "false";
  } else if (v.ty == &charstring_typeinfo) {
    auto* s = static_cast<vrt_charstring_t*>(v.p);
    buf += "\"";
    buf += std::string_view{vrt_charstring_get_cbuf(s), s->length};
    buf += "\"";
  } else if (v.ty == &octetstring_typeinfo) {
    auto* s = static_cast<vrt_octetstring_t*>(v.p);
    const auto* sdata = vrt_octetstring_get_cbuf(s);
    buf += "'";
    for (std::uint32_t i = 0; i < s->length; ++i) {
      const auto& octet = sdata[i];
      buf += kHexDigits[octet >> 4];
      buf += kHexDigits[octet & 0x0F];
    }
    buf += "'O";
  } else if (v.ty == &hexstring_typeinfo) {
    auto* s = static_cast<vrt_hexstring_t*>(v.p);
    const auto* sdata = vrt_hexstring_get_cbuf(s);
    buf += "'";
    for (std::uint32_t i = 0; i < s->length; ++i) {
      const auto& octet = sdata[i / 2];
      const std::uint8_t nib = (i & 1) ? (octet & 0xF) : ((octet >> 4) & 0xF);
      buf += kHexDigits[nib];
    }
    buf += "'H";
  } else if (v.ty == &bitstring_typeinfo) {
    auto* s = static_cast<vrt_bitstring_t*>(v.p);
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
}

}  // namespace vanadium::rt
