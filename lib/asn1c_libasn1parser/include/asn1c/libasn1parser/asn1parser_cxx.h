#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc99-designator"
#pragma GCC diagnostic ignored "-Wwritable-strings"

extern "C" {
// clang-format off
#include "asn1parser.h"                      // IWYU pragma: export

#include "asn1c/libasn1common/asn1_ref.h"    // IWYU pragma: export
#include "asn1p_alloc.h"                     // IWYU pragma: export
#include "asn1p_integer.h"                   // IWYU pragma: export
#include "asn1p_list.h"                      // IWYU pragma: export
#include "asn1p_oid.h"		                   // IWYU pragma: export
#include "asn1p_module.h"	                   // IWYU pragma: export
#include "asn1p_value.h"	                   // IWYU pragma: export
#include "asn1p_param.h"	                   // IWYU pragma: export
#include "asn1p_constr.h"	                   // IWYU pragma: export
#include "asn1p_xports.h"	                   // IWYU pragma: export
#include "asn1p_class.h"	                   // IWYU pragma: export
#include "asn1p_expr.h"		                   // IWYU pragma: export
// clang-format on
}

#pragma GCC diagnostic pop
