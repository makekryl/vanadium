#pragma once

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <string>
#include <string_view>
#include <vector>

#include "FunctionRef.h"

namespace vanadium::core {
namespace ast {

using pos_t = std::uint32_t;

struct Range {
  pos_t begin;
  pos_t end;

  [[nodiscard]] bool Contains(const Range& other) const noexcept {
    return begin <= other.begin && other.end <= end;
  }

  [[nodiscard]] bool Contains(pos_t pos) const noexcept {
    return begin <= pos && pos <= end;
  }

  [[nodiscard]] pos_t Length() const noexcept {
    return end - begin;
  }

  [[nodiscard]] std::string_view String(std::string_view src) const noexcept {
    return src.substr(begin, end - begin);
  }

  [[nodiscard]] auto operator<=>(const Range&) const noexcept = default;
};

struct Location {
  pos_t line;
  pos_t column;
};

class LineMapping {
 public:
  LineMapping() = default;  // TODO: delete

  LineMapping(std::vector<pos_t>&& line_starts) : line_starts_(std::move(line_starts)) {}

  [[nodiscard]] pos_t StartOf(pos_t line) const {
    return line_starts_[line];
  }

  [[nodiscard]] pos_t LineOf(pos_t pos) const {
    const auto it = std::ranges::upper_bound(line_starts_, pos);
    if (it == line_starts_.end()) [[unlikely]] {
      return line_starts_.size() - 1;
    }

    pos_t line = std::distance(line_starts_.begin(), it);
    if ((line != 0) && (pos != *it)) [[likely]] {
      --line;
    }
    return line;
  }

  [[nodiscard]] Location Translate(pos_t pos) const {
    const pos_t line = LineOf(pos);
    return {
        .line = line,
        .column = pos - StartOf(line),
    };
  }

  [[nodiscard]] pos_t GetPosition(Location loc) const {
    return StartOf(loc.line) + loc.column;
  }

  [[nodiscard]] std::size_t Count() const {
    return line_starts_.size();
  }

 private:
  std::vector<pos_t> line_starts_;
};

struct SyntaxError {
  ast::Range range;
  std::string description;
};

struct Node;

using NodeInspector = lib::Predicate<const Node*>;
void Inspect(const Node*, const NodeInspector&);

}  // namespace ast
};  // namespace vanadium::core
