#pragma once

#include <iostream>  // IWYU pragma: export

#define DEFINE_NAMED_TEST_PARAM_PRINTER(TPARAM, MEMBER)          \
  std::ostream& operator<<(std::ostream& out, const TPARAM& p) { \
    out << p.MEMBER;                                             \
    return out;                                                  \
  }
