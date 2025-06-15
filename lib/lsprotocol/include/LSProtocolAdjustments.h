#pragma once

#include "LSProtocol.h"

// NOLINTBEGIN(readability-identifier-naming)

namespace glz {
template <>
struct from<JSON, lsp::TextDocumentContentChangeEvent> {
  template <auto Opts>
  static void op(lsp::TextDocumentContentChangeEvent& value, auto&& ctx, auto&& it, auto&& end) {
    struct TextDocumentContentChangePartial_or_TextDocumentContentChangeWholeDocument {
      std::optional<lsp::Range> range;
      std::optional<std::uint32_t> rangeLength;
      std::string text;
    } deduce;

    parse<JSON>::op<Opts>(deduce, ctx, it, end);
    if (deduce.range) {
      value = lsp::TextDocumentContentChangePartial{
          .range = *deduce.range,
          .rangeLength = *deduce.rangeLength,
          .text = deduce.text,
      };
    } else {
      value = lsp::TextDocumentContentChangeWholeDocument{.text = deduce.text};
    }
  }
};
}  // namespace glz

// NOLINTEND(readability-identifier-naming)
