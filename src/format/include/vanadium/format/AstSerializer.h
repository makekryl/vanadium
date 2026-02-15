#pragma once

#include <cstdint>
#include <string_view>
#include <variant>
#include <vector>

#include <vanadium/lib/EnumFlags.h>

namespace vanadium {

namespace ast {
struct AST;
struct Node;
}  // namespace ast

namespace lib {
class Arena;
}

namespace format {

enum class PrintDirective : std::uint8_t {
  kSpace,
  kSpaceOrLine,

  kHardLine,
  kSoftLine,

  kSemicolon,
};

struct Comment {
  std::string_view content;
};

struct PreferredNewlines {
  std::size_t count;
};

struct Sequence;

using EmptyUnit = std::monostate;
using Unit = std::variant<const Sequence*, PrintDirective, std::string_view, Comment, PreferredNewlines, EmptyUnit>;

struct Sequence {
  enum class Attribute : std::uint8_t {
    kNone = 0,
    kGrouped = 1 << 0,
    kIndented = 1 << 1,
    kNoWrap = 1 << 2,
  } attributes;
  std::vector<Unit> units;
};
ENUM_FLAGS_TRAITS(Sequence::Attribute);

Unit SerializeAst(const ast::AST& ast, const ast::Node* n, lib::Arena& arena);

}  // namespace format

}  // namespace vanadium
