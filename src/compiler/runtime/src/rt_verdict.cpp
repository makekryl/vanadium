#include "vanadium/runtime/rt_verdict.h"

#include <magic_enum/magic_enum.hpp>

#include "vanadium/runtime/runtime.hpp"

namespace {
vrt_verdicttype g_verdict{vrt_verdicttype::none};
}  // namespace

// ES 201 873-1 [TTCN-3: Core Language], par. 24.1
void vrt_setverdict(const vrt_int_t* verdict, [[maybe_unused]] const vrt_charstring_t* reason) {
  // TODO(compiler): pass int, float, bool by value
  const auto v = static_cast<vrt_verdicttype>(verdict->value);
  if (g_verdict < v) {
    // TODO: save and log reason
    rt::Log("setverdict({}): {} -> {}", magic_enum::enum_name(v), magic_enum::enum_name(g_verdict),
            magic_enum::enum_name(v));
    g_verdict = v;
  }
}

vrt_verdicttype vrt_getverdict() {
  return g_verdict;
}

void vrt_clearverdict() {
  g_verdict = vrt_verdicttype::none;
}
