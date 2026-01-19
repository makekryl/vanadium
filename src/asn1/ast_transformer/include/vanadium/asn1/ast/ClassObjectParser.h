#pragma once

#include <vanadium/lib/FunctionRef.h>

#include <string_view>

using asn1p_wsyntx_t = struct asn1p_wsyntx_s;

namespace vanadium::asn1::ast {

struct ClassObjectRow {
  std::string_view name;
  std::string_view value;

  bool operator<=>(const ClassObjectRow&) const noexcept = default;
};

// TODO: emit errors
struct ClassObjectConsumer {
  lib::Predicate<ClassObjectRow> accept_row;
  // lib::Consumer<std::string> accept_error;
};

void ParseClassObject(std::string_view buf, const asn1p_wsyntx_t* syntax, const ClassObjectConsumer&);

}  // namespace vanadium::asn1::ast
