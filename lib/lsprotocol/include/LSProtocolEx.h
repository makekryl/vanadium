#pragma once

#include <cstddef>

#include "LSProtocol.h"

// NOLINTBEGIN(readability-identifier-naming)

// Server -> Client
namespace lsp {
using CodeActionResult = std::variant<Command, std::vector<CodeAction>, std::nullptr_t>;
using DefinitionResult = std::variant<Location, std::vector<Location>, std::vector<LocationLink>, std::nullptr_t>;
using ReferencesResult = std::variant<std::vector<Location>, std::nullptr_t>;
using TypeDefinitionResult = std::variant<Location, std::vector<Location>, std::vector<LocationLink>, std::nullptr_t>;
using DocumentHighlightResults = std::variant<std::vector<DocumentHighlight>, std::nullptr_t>;
using HoverResult = std::variant<Hover, std::nullptr_t>;
using RenameResult = std::variant<WorkspaceEdit, std::nullptr_t>;
using CompletionResult = std::variant<std::vector<CompletionItem>, CompletionList, std::nullptr_t>;
using InlayHintResult = std::variant<std::vector<InlayHint>, std::nullptr_t>;
using DocumentSybmolResult = std::variant<std::vector<DocumentSymbol>, std::vector<SymbolInformation>, std::nullptr_t>;
using SignatureHelpResult = std::variant<SignatureHelp, std::nullptr_t>;
using SemanticTokensRangeResult = std::variant<SemanticTokens, std::nullptr_t>;
}  // namespace lsp

// Client -> Server
namespace lsp {
using ShowMessageRequestResult = std::variant<MessageActionItem, std::nullptr_t>;
}

// NOLINTEND(readability-identifier-naming)
