#include <cassert>
#include <cmath>
#include <format>
#include <limits>
#include <string>

#include "vanadium/runtime/BuiltinsTemplates.h"
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
#include "vanadium/runtime/runtime.h"

namespace vanadium::rt {

namespace {
constexpr auto kHexDigits = "0123456789ABCDEF";

void StringifyValue(std::string& buf, const vrt_integer_t& v) {
  buf += std::to_string(v.value);
}
void StringifyTemplate(std::string& buf, const vrt_integer_template_t& t) {
  switch (t.tsel) {
    case vrt_template_sel_e::kValueRange:
      buf += "(";
      if (t.vmin_exclusive) {
        buf += "!";
      }
      if (t.vmin_present) {
        StringifyValue(buf, vrt_integer_wrap(t.range.vmin));
      } else {
        buf += "-infinity";
      }
      buf += " .. ";
      if (t.vmax_exclusive) {
        buf += "!";
      }
      if (t.vmax_present) {
        StringifyValue(buf, vrt_integer_wrap(t.range.vmax));
      } else {
        buf += "infinity";
      }
      buf += ")";
      break;
    default:
      assert(false);
      break;
  }
}

void StringifyValue(std::string& buf, const vrt_float_t& v) {
  const auto val = v.value;
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
}
void StringifyTemplate(std::string& buf, const vrt_float_template_t& t) {}

void StringifyValue(std::string& buf, const vrt_boolean_t& v) {
  buf += v.value ? "true" : "false";
}
void StringifyTemplate(std::string& buf, const vrt_boolean_template_t& t) {}

void StringifyValue(std::string& buf, const vrt_charstring_t& v) {
  buf += "\"";
  buf += std::string_view{vrt_charstring_get_cbuf(&v), v.length};
  buf += "\"";
}
void StringifyTemplate(std::string& buf, const vrt_charstring_template_t& t) {}

void StringifyValue(std::string& buf, const vrt_octetstring_t& v) {
  const auto* sdata = vrt_octetstring_get_cbuf(&v);
  buf += "'";
  for (std::uint32_t i = 0; i < v.length; ++i) {
    const auto& octet = sdata[i];
    buf += kHexDigits[octet >> 4];
    buf += kHexDigits[octet & 0x0F];
  }
  buf += "'O";
}
void StringifyTemplate(std::string& buf, const vrt_octetstring_template_t& t) {}

void StringifyValue(std::string& buf, const vrt_bitstring_t& v) {
  const auto* sdata = vrt_bitstring_get_cbuf(&v);
  buf += "'";
  //
  const auto full_bytes = v.length / 8;
  const auto remaining = v.length % 8;
  //
  for (size_t i = 0; i < full_bytes; ++i) {
    const std::uint8_t byte = sdata[i];
    for (int b = 7; b >= 0; --b) {
      buf += ((byte >> b) & 1) ? '1' : '0';
    }
  }
  if (remaining) {
    const std::uint8_t byte = sdata[full_bytes];
    for (bitstring_size_t b = 0; b < remaining; ++b) {
      buf += ((byte >> (7 - b)) & 1) ? '1' : '0';
    }
  }
  //
  buf += "'B";
}
void StringifyTemplate(std::string& buf, const vrt_bitstring_template_t& t) {}

void StringifyValue(std::string& buf, const vrt_hexstring_t& v) {
  const auto* sdata = vrt_hexstring_get_cbuf(&v);
  buf += "'";
  for (std::uint32_t i = 0; i < v.length; ++i) {
    const auto& octet = sdata[i / 2];
    const std::uint8_t nib = (i & 1) ? (octet & 0xF) : ((octet >> 4) & 0xF);
    buf += kHexDigits[nib];
  }
  buf += "'H";
}
void StringifyTemplate(std::string& buf, const vrt_hexstring_template_t& t) {}

//

void StringifyTemplateGeneric(std::string& buf, const tpl::RtTemplate auto& t) {
  const auto stringify_value_list = [&] {
    buf += "(";
    auto range = t.list.Range();
    auto* it = range.begin();
    auto* end = range.end();
    if (it != end) {
      StringifyTemplateGeneric(buf, *it++);
    }
    for (; it != end; ++it) {
      buf += ", ";
      StringifyTemplateGeneric(buf, *it);
    }
    buf += ")";
  };

  switch (t.tsel) {
    case vrt_template_sel_e::kSpecificValue:
      StringifyValue(buf, t.val);
      break;
    case vrt_template_sel_e::kComplementedList:
      buf += "complement";
      stringify_value_list();
      break;
    case vrt_template_sel_e::kConjunctionList:
      buf += "conjunct";
      stringify_value_list();
      break;
    case vrt_template_sel_e::kValueList:
      stringify_value_list();
      break;
    case vrt_template_sel_e::kImplication:
      StringifyTemplateGeneric(buf, t.implication->precondition);
      buf += " implies ";
      StringifyTemplateGeneric(buf, t.implication->implied);
      break;
    case vrt_template_sel_e::kDynamic:
      buf += "@dynamic template";
      break;
    default:
      StringifyTemplate(buf, t);
      break;
  }
}

void StringifyUnionReflect(std::string& buf, const vrt_val_t& v) {
  const auto active_idx = GetUnionSelection(v);
  const auto& m = v.ty->members[active_idx];

  buf += "{ ";
  buf += m.name;
  buf += " := ";
  StringifyObject(buf, {.p = GetMemberPtr(m, v.p), .ty = m.type});
  buf += " }";
}

void StringifyStructReflect(std::string& buf, const vrt_val_t& v) {
  if (v.ty->kind == vrt_typekind_e::kUnion) {
    StringifyUnionReflect(buf, v);
    return;
  }

  buf += "{ ";
  const std::span msp{v.ty->members, v.ty->members_count};
  for (const auto& m : msp) {
    // printf(" --> chk [%p] %s of type %s w/ off=%zu tp=%d\n", v.p, m.name, m.type->name, m.offset, m.type->kind);
    buf += m.name;
    buf += " := ";
    StringifyObject(buf, {.p = GetMemberPtr(m, v.p), .ty = m.type});

    buf += ", ";
  }
  if (!msp.empty()) [[likely]] {
    buf.erase(buf.size() - 2);
  }
  buf += " }";
}

void StringifyReflect(std::string& buf, const vrt_val_t& v) {
  switch (v.ty->kind) {
    case vrt_typekind_e::kRecord:
    case vrt_typekind_e::kSet:
    case vrt_typekind_e::kUnion: {
      if (tpl::IsTemplateType(v.ty)) {
        auto* t = static_cast<tpl::GenericTemplateType*>(v.p);
        const auto stringify_value_list = [&] {
          buf += "(";
          auto range = t->list.Range();
          auto it = range.begin();
          auto end = range.end();
          if (it != end) {
            StringifyReflect(buf, {.p = &(*it++), .ty = v.ty});
          }
          for (; it != end; ++it) {
            buf += ", ";
            StringifyReflect(buf, {.p = &(*it), .ty = v.ty});
          }
          buf += ")";
        };
        switch (t->tsel) {
          case vrt_template_sel_e::kSpecificValue:
            StringifyStructReflect(buf, v);
            break;
          case vrt_template_sel_e::kComplementedList:
            buf += "complement";
            stringify_value_list();
            break;
          case vrt_template_sel_e::kConjunctionList:
            buf += "conjunct";
            stringify_value_list();
            break;
          case vrt_template_sel_e::kValueList:
            stringify_value_list();
            break;
          case vrt_template_sel_e::kImplication:
            StringifyReflect(buf, {.p = &t->implication->precondition, .ty = v.ty});
            buf += " implies ";
            StringifyReflect(buf, {.p = &t->implication->implied, .ty = v.ty});
            break;
          case vrt_template_sel_e::kDynamic:
            buf += "@dynamic template";
            break;
          default:
            assert(false);
            break;
        }
      } else {
        StringifyStructReflect(buf, v);
      }
      break;
    }
    default:
      assert(false);
      break;
  }
}

}  // namespace

void StringifyObject(std::string& buf, const vrt_val_t& v) {
  // printf("stringify(%s, %p)\n", v.ty->name, v.p);
  if (!vrt_is_bound(&v)) {
    buf += tpl::IsTemplateType(v.ty) ? "<uninitialized template>" : "<unbound>";
    return;
  }

  if (tpl::IsTemplateType(v.ty)) {
#define X(name)                                                                       \
  if (v.ty == &name##_template_typeinfo) {                                            \
    StringifyTemplateGeneric(buf, *static_cast<const vrt_##name##_template_t*>(v.p)); \
    return;                                                                           \
  }
#include "vanadium/runtime/BuiltinTypes.inc"
#undef X
  } else {
#define X(name)                                                    \
  if (v.ty == &name##_typeinfo) {                                  \
    StringifyValue(buf, *static_cast<const vrt_##name##_t*>(v.p)); \
    return;                                                        \
  }
#include "vanadium/runtime/BuiltinTypes.inc"
#undef X
  }

  // TODO: use switch(kind)-case instead of pointer comparison above
  StringifyReflect(buf, v);
}

}  // namespace vanadium::rt
