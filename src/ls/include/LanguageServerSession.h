#pragma once

namespace vanadium {

namespace tooling {
class Solution;
}

namespace lint {
class Linter;
}

namespace lib {
class Arena;
}

namespace ls {
struct LsSessionRef {
  const tooling::Solution& solution;
  const lint::Linter& linter;
  lib::Arena& arena;
};
}  // namespace ls

}  // namespace vanadium
