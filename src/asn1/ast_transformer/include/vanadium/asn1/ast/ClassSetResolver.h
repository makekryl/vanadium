#pragma once

#include <string>

#include <vanadium/lib/FunctionRef.h>

#include "vanadium/asn1/ast/ClassObjectParser.h"

using asn1p_expr_t = struct asn1p_expr_s;
using asn1p_ref_t = struct asn1p_ref_s;
using asn1p_src_range_t = struct asn1p_src_range_s;

namespace vanadium::asn1::ast {

struct ClassSetElementConsumer {
  lib::FunctionRef<const asn1p_expr_t*(const asn1p_ref_t*)> resolve;
  lib::Predicate<lib::Consumer<ClassObjectConsumer> /* inspect */> accept_class;
  lib::Consumer<const asn1p_src_range_t&, std::string> emit_error;
};

void ResolveClassSet(const asn1p_expr_t* s_expr, const asn1p_expr_t* cls_expr, const ClassSetElementConsumer&);

}  // namespace vanadium::asn1::ast
