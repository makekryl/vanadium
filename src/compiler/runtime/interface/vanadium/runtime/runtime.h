#pragma once

#include "vanadium/runtime/rt_alloc.h"        // IWYU pragma: export
#include "vanadium/runtime/rt_bitstring.h"    // IWYU pragma: export
#include "vanadium/runtime/rt_boolean.h"      // IWYU pragma: export
#include "vanadium/runtime/rt_charstring.h"   // IWYU pragma: export
#include "vanadium/runtime/rt_float.h"        // IWYU pragma: export
#include "vanadium/runtime/rt_hexstring.h"    // IWYU pragma: export
#include "vanadium/runtime/rt_integer.h"      // IWYU pragma: export
#include "vanadium/runtime/rt_octetstring.h"  // IWYU pragma: export
#include "vanadium/runtime/rt_optional.h"     // IWYU pragma: export
#include "vanadium/runtime/rt_reflect.h"      // IWYU pragma: export
#include "vanadium/runtime/rt_template.h"     // IWYU pragma: export

// NOLINTBEGIN(readability-identifier-naming)

extern "C" {
//

void vrt_panic(const char*);

bool vrt_is_bound(const vrt_val_t*);

void vrt_log(const vrt_val_t*, std::uint32_t);
void vrt_log_write(const char*);

//
}

// NOLINTEND(readability-identifier-naming)
