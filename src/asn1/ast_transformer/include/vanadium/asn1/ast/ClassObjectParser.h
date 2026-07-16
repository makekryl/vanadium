#pragma once

#include <string_view>

#include <asn1c/libasn1common/asn1p_src_range.h>

#include <vanadium/lib/FunctionRef.h>

using asn1p_wsyntx_t = struct asn1p_wsyntx_s;

namespace vanadium::asn1::ast {

struct ClassObjectRow {
  std::string_view name;
  std::string_view value;
  asn1p_src_range_t range{};
};

// TODO: emit errors
struct ClassObjectConsumer {
  lib::Predicate<ClassObjectRow> accept_row;
  lib::Consumer<const asn1p_src_range_t&, std::string> emit_error;
  asn1p_src_range_t range{};
};

void ParseClassObject(std::string_view buf, const asn1p_wsyntx_t* syntax, const ClassObjectConsumer&);

}  // namespace vanadium::asn1::ast
