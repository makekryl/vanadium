#pragma once

#include "vanadium/format/AstPrinter.h"
namespace vanadium {

namespace tooling {
class Solution;
}

namespace lint {
class Linter;
}

namespace format {
struct PrintOptions;
}

namespace lib {
class Arena;
}

namespace ls {
struct LsSessionRef {
  const tooling::Solution& solution;
  lib::Arena& arena;
  struct {
    const lint::Linter& linter;
    const format::PrintOptions* fmt_opts;
  } tools;
};
}  // namespace ls

}  // namespace vanadium
