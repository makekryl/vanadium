#pragma once

#include <cstddef>
#include <glaze/util/string_literal.hpp>

#include "LSMethod.h"
#include "LSProtocol.h"
#include "LSProtocolEx.h"
#include "LanguageServerContext.h"

namespace vanadium::ls {

namespace rpc = lserver::rpc;

// subclass over using-alias to avoid too much noise from templates in stacktraces
#define DECL_METHOD(NAME, HANDLE, PARAMS, RESULT, RETVAL) \
  struct NAME : rpc::Method<HANDLE, PARAMS, RESULT> {     \
    static RETVAL invoke(LsContext&, const PARAMS&);      \
  };

#define DECL_REQUEST_0(NAME, PARAMS, RESULT) DECL_METHOD(NAME, #NAME, PARAMS, RESULT, rpc::ExpectedResult<RESULT>)
#define DECL_REQUEST_1(W0, NAME, PARAMS, RESULT)                                \
  namespace W0 {                                                                \
  DECL_METHOD(NAME, #W0 "/" #NAME, PARAMS, RESULT, rpc::ExpectedResult<RESULT>) \
  }
#define DECL_REQUEST_2(W0, W1, NAME, PARAMS, RESULT)                                    \
  namespace W0::W1 {                                                                    \
  DECL_METHOD(NAME, #W0 "/" #W1 "/" #NAME, PARAMS, RESULT, rpc::ExpectedResult<RESULT>) \
  }

#define DECL_NOTIFIC_0(NAME, PARAMS) DECL_METHOD(NAME, #NAME, PARAMS, lib::jsonrpc::Empty, void)
#define DECL_NOTIFIC_1(W0, NAME, PARAMS)                              \
  namespace W0 {                                                      \
  DECL_METHOD(NAME, #W0 "/" #NAME, PARAMS, lib::jsonrpc::Empty, void) \
  }
#define DECL_NOTIFIC_2(W0, W1, NAME, PARAMS)                                  \
  namespace W0::W1 {                                                          \
  DECL_METHOD(NAME, #W0 "/" #W1 "/" #NAME, PARAMS, lib::jsonrpc::Empty, void) \
  }

// NOLINTBEGIN(readability-identifier-naming)

namespace methods {

DECL_REQUEST_0(initialize, lsp::InitializeParams, lsp::InitializeResult);
DECL_NOTIFIC_0(initialized, lib::jsonrpc::Empty);
DECL_REQUEST_0(shutdown, lib::jsonrpc::Empty, std::nullptr_t);
DECL_NOTIFIC_0(exit, lib::jsonrpc::Empty);

namespace dollar {
DECL_METHOD(cancelRequest, "$/cancelRequest", lsp::CancelParams, lib::jsonrpc::Empty, void)
DECL_METHOD(setTrace, "$/setTrace", lsp::SetTraceParams, lib::jsonrpc::Empty, void)
}  // namespace dollar

// textDocument
DECL_NOTIFIC_1(textDocument, didOpen, lsp::DidOpenTextDocumentParams);
DECL_NOTIFIC_1(textDocument, didChange, lsp::DidChangeTextDocumentParams);
DECL_NOTIFIC_1(textDocument, didSave, lsp::DidSaveTextDocumentParams);
DECL_NOTIFIC_1(textDocument, didClose, lsp::DidCloseTextDocumentParams);
DECL_REQUEST_1(textDocument, diagnostic, lsp::DocumentDiagnosticParams, lsp::DocumentDiagnosticReport);
DECL_REQUEST_1(textDocument, codeAction, lsp::CodeActionParams, lsp::CodeActionResult);
DECL_REQUEST_1(textDocument, definition, lsp::DefinitionParams, lsp::DefinitionResult);
DECL_REQUEST_1(textDocument, references, lsp::ReferenceParams, lsp::ReferencesResult);
DECL_REQUEST_1(textDocument, typeDefinition, lsp::TypeDefinitionParams, lsp::TypeDefinitionResult);
DECL_REQUEST_1(textDocument, documentHighlight, lsp::DocumentHighlightParams, lsp::DocumentHighlightResults);
DECL_REQUEST_1(textDocument, hover, lsp::HoverParams, lsp::HoverResult);
DECL_REQUEST_1(textDocument, rename, lsp::RenameParams, lsp::RenameResult);
DECL_REQUEST_1(textDocument, completion, lsp::CompletionParams, lsp::CompletionResult);
DECL_REQUEST_1(textDocument, inlayHint, lsp::InlayHintParams, lsp::InlayHintResult);
DECL_REQUEST_1(textDocument, documentSymbol, lsp::DocumentSymbolParams, lsp::DocumentSybmolResult);
DECL_REQUEST_1(textDocument, signatureHelp, lsp::SignatureHelpParams, lsp::SignatureHelpResult);
DECL_REQUEST_2(textDocument, semanticTokens, range, lsp::SemanticTokensRangeParams, lsp::SemanticTokensRangeResult);

// workspace
DECL_NOTIFIC_1(workspace, didChangeWatchedFiles, lsp::DidChangeWatchedFilesParams);

// completionItem
DECL_REQUEST_1(completionItem, resolve, lsp::CompletionItem, lsp::CompletionItem);

// inlayHint
DECL_REQUEST_1(inlayHint, resolve, lsp::InlayHint, lsp::InlayHint);

}  // namespace methods

#undef DECL_NOTIFIC_2
#undef DECL_NOTIFIC_1
#undef DECL_NOTIFIC_0
#undef DECL_REQUEST_2
#undef DECL_REQUEST_1
#undef DECL_REQUEST_0
#undef DECL_METHOD

// NOLINTEND(readability-identifier-naming)

}  // namespace vanadium::ls
