#pragma once

#include <iostream>  // IWYU pragma: export
#include <string_view>

#define DEFINE_NAMED_TEST_PARAM_PRINTER(TPARAM, MEMBER)                 \
  inline std::ostream& operator<<(std::ostream& out, const TPARAM& p) { \
    out << p.MEMBER;                                                    \
    return out;                                                         \
  }

// TODO: move it out of here
struct NamedStringParam {
  std::string_view name;
  std::string_view value;

  operator std::string_view() const {
    return value;
  }

  constexpr auto operator<=>(const NamedStringParam& other) const noexcept(noexcept(value <=> other.value)) {
    return value <=> other.value;
  }
};
DEFINE_NAMED_TEST_PARAM_PRINTER(NamedStringParam, name)
