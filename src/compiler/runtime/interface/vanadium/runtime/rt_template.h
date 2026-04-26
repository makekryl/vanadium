#pragma once

#include <cstdint>

enum class vrt_template_sel_e : std::uint8_t {
  kUninitialized = 0,

  kSpecificValue,

  kOmitValue,
  kAnyValue,
  kAnyOrOmit,

  kValueList,
  kComplementedList,

  kValueRange,
  kStringPattern,

  kSupersetMatch,
  kSubsetMatch,

  kDecodeMatch,

  kConjunctionMatch,
  kImplicationMatch,
  kDynamicMatch,
};
