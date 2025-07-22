#pragma once

#include <cstddef>

#include "LSProtocol.h"

// NOLINTBEGIN(readability-identifier-naming)

namespace lsp {
using CodeActionResult = std::variant<Command, std::vector<CodeAction>, std::nullptr_t>;
using DefinitionResult = std::variant<Location, std::vector<Location>, std::vector<LocationLink>, std::nullptr_t>;
using ReferencesResult = std::variant<std::vector<Location>, std::nullptr_t>;
using TypeDefinitionResult = std::variant<Location, std::vector<Location>, std::vector<LocationLink>, std::nullptr_t>;
using DocumentHighlightResults = std::variant<std::vector<DocumentHighlight>, std::nullptr_t>;
using HoverResult = std::variant<Hover, std::nullptr_t>;
using RenameResult = std::variant<WorkspaceEdit, std::nullptr_t>;
using CompletionResult = std::variant<std::vector<CompletionItem>, CompletionList, std::nullptr_t>;
using CompletionResolutionResult = std::variant<CompletionItem, std::nullptr_t>;
using InlayHintResult = std::variant<std::vector<InlayHint>, std::nullptr_t>;
}  // namespace lsp

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
