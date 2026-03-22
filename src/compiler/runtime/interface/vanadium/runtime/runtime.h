#pragma once

#include "vanadium/runtime/rt_alloc.h"       // IWYU pragma: export
#include "vanadium/runtime/rt_boolean.h"     // IWYU pragma: export
#include "vanadium/runtime/rt_charstring.h"  // IWYU pragma: export
#include "vanadium/runtime/rt_integer.h"     // IWYU pragma: export
#include "vanadium/runtime/rt_optional.h"    // IWYU pragma: export
#include "vanadium/runtime/rt_reflect.h"     // IWYU pragma: export

// NOLINTBEGIN(readability-identifier-naming)

enum class vrt_verdicttype_t : std::uint8_t {
  kNone,
  kPass,
  kInconc,
  kFail,
  kError,
};

extern "C" {
//

void vrt_panic(const char*);

void vrt_log(const vrt_val_t*, std::uint32_t);

//
}

// NOLINTEND(readability-identifier-naming)
