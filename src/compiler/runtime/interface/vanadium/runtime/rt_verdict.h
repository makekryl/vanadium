#pragma once

#include "vanadium/runtime/rt_charstring.h"
#include "vanadium/runtime/rt_integer.h"

// NOLINTBEGIN(readability-identifier-naming)

// NOLINTNEXTLINE(performance-enum-size)
enum class vrt_verdicttype : vrt_native_int_t {
  none = 0,
  pass,
  inconc,
  fail,
  error,
};

extern "C" {
//

void vrt_setverdict(const vrt_integer_t* verdict, const vrt_charstring_t* reason);
vrt_verdicttype vrt_getverdict();

void vrt_clearverdict();

//
}

// NOLINTEND(readability-identifier-naming)
