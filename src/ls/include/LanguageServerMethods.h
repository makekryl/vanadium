#pragma once

#include <cstddef>
#include <glaze/ext/jsonrpc.hpp>
#include <glaze/util/expected.hpp>

#include "LSMethod.h"
#include "LSProtocol.h"
#include "LanguageServerContext.h"

namespace vanadium::ls {

namespace rpc {

using namespace lserver::rpc;

template <glz::string_literal Name, typename Params, typename Result>
struct Request : Method<Name, Params, Result> {
  ExpectedResult<Result> operator()(VanadiumLsContext&, const Params&);
};

template <glz::string_literal Name, typename Params, typename Result>
struct Notification : Method<Name, Params, Result> {
  void operator()(VanadiumLsContext&, const Params&);
};

}  // namespace rpc

#define DECL_METHOD_0(TYPE, PARAMS, RESULT, NAME) using NAME = rpc::TYPE<#NAME, PARAMS, RESULT>
#define DECL_METHOD_1(TYPE, PARAMS, RESULT, W0, NAME)    \
  namespace W0 {                                         \
  using NAME = rpc::TYPE<#W0 "/" #NAME, PARAMS, RESULT>; \
  }
#define DECL_METHOD_2(TYPE, PARAMS, RESULT, W0, W1, NAME)        \
  namespace W0::W1 {                                             \
  using NAME = rpc::TYPE<#W0 "/" #W1 "/" #NAME, PARAMS, RESULT>; \
  }

#define DECL_REQUEST_0(PARAMS, RESULT, NAME) DECL_METHOD_0(Request, PARAMS, RESULT, NAME)
#define DECL_REQUEST_1(PARAMS, RESULT, W0, NAME) DECL_METHOD_1(Request, PARAMS, RESULT, W0, NAME)
#define DECL_REQUEST_2(PARAMS, RESULT, W0, W1, NAME) DECL_METHOD_2(Request, PARAMS, RESULT, W0, W1, NAME)

#define DECL_NOTIFIC_0(PARAMS, NAME) DECL_METHOD_0(Notification, PARAMS, std::nullptr_t, NAME)
#define DECL_NOTIFIC_1(PARAMS, W0, NAME) DECL_METHOD_1(Notification, PARAMS, std::nullptr_t, W0, NAME)
#define DECL_NOTIFIC_2(PARAMS, W0, W1, NAME) DECL_METHOD_2(Notification, PARAMS, std::nullptr_t, W0, W1, NAME)

// NOLINTBEGIN(readability-identifier-naming)

namespace methods {

DECL_REQUEST_0(lsp::InitializeParams, lsp::InitializeResult, initialize);
DECL_NOTIFIC_0(rpc::Empty, initialized);
DECL_REQUEST_0(rpc::Empty, std::nullptr_t, shutdown);
DECL_NOTIFIC_0(rpc::Empty, exit);

// textDocument
DECL_NOTIFIC_1(lsp::DidOpenTextDocumentParams, textDocument, didOpen);
DECL_NOTIFIC_1(lsp::DidChangeTextDocumentParams, textDocument, didChange);
DECL_REQUEST_1(lsp::DocumentDiagnosticParams, lsp::DocumentDiagnosticReport, textDocument, diagnostic);
DECL_REQUEST_1(lsp::CodeActionParams, int, textDocument,
               codeAction);  // std::variant<lsp::Command, std::vector<lsp::CodeAction>, std::nullptr_t>

// workspace
DECL_REQUEST_1(lsp::WorkspaceDiagnosticParams, lsp::WorkspaceDiagnosticReport, workspace, diagnostic);

}  // namespace methods

#undef DECL_NOTIFIC_2
#undef DECL_NOTIFIC_1
#undef DECL_NOTIFIC_0
#undef DECL_REQUEST_2
#undef DECL_REQUEST_1
#undef DECL_REQUEST_0
#undef DECL_METHOD_2
#undef DECL_METHOD_1
#undef DECL_METHOD_0

// NOLINTEND(readability-identifier-naming)

}  // namespace vanadium::ls
