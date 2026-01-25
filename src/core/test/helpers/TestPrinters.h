#pragma once
// IWYU pragma: always_keep

#include <iostream>
#include <vector>

#include <magic_enum/magic_enum.hpp>

#include <vanadium/ast/ASTTypes.h>
#include <vanadium/core/Semantic.h>

namespace vanadium {

namespace ast {
inline std::ostream& operator<<(std::ostream& out, const SyntaxError& e) {
  out << e.range.begin << ".." << e.range.end << ": " << e.description;
  return out;
}
inline std::ostream& operator<<(std::ostream& out, const std::vector<SyntaxError>& v) {
  out << "Syntax errors:\n";
  for (const auto& e : v) {
    out << " - " << e << "\n";
  }
  return out;
}
}  // namespace ast

namespace core {
namespace semantic {
inline std::ostream& operator<<(std::ostream& out, const SemanticError& e) {
  out << e.range.begin << ".." << e.range.end << ": " << magic_enum::enum_name(e.type);
  return out;
}
inline std::ostream& operator<<(std::ostream& out, const std::vector<SemanticError>& v) {
  out << "Semantic errors:\n";
  for (const auto& e : v) {
    out << " - " << e << "\n";
  }
  return out;
}
}  // namespace semantic

}  // namespace core
}  // namespace vanadium
