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
  ExpectedResult<Result> operator()(LsContext&, const Params&);
};

template <glz::string_literal Name, typename Params, typename Result>
struct Notification : Method<Name, Params, Result> {
  void operator()(LsContext&, const Params&);
};

}  // namespace rpc

#define DECL_METHOD_0(NAME, TYPE, PARAMS, RESULT) using NAME = rpc::TYPE<#NAME, PARAMS, RESULT>
#define DECL_METHOD_1(W0, NAME, TYPE, PARAMS, RESULT)    \
  namespace W0 {                                         \
  using NAME = rpc::TYPE<#W0 "/" #NAME, PARAMS, RESULT>; \
  }
#define DECL_METHOD_2(W0, W1, NAME, TYPE, PARAMS, RESULT)        \
  namespace W0::W1 {                                             \
  using NAME = rpc::TYPE<#W0 "/" #W1 "/" #NAME, PARAMS, RESULT>; \
  }

#define DECL_REQUEST_0(NAME, PARAMS, RESULT) DECL_METHOD_0(NAME, Request, PARAMS, RESULT)
#define DECL_REQUEST_1(W0, NAME, PARAMS, RESULT) DECL_METHOD_1(W0, NAME, Request, PARAMS, RESULT)
#define DECL_REQUEST_2(W0, W1, NAME, PARAMS, RESULT) DECL_METHOD_2(W0, W1, NAME, Request, PARAMS, RESULT)

#define DECL_NOTIFIC_0(NAME, PARAMS) DECL_METHOD_0(NAME, Notification, PARAMS, std::nullptr_t)
#define DECL_NOTIFIC_1(W0, NAME, PARAMS) DECL_METHOD_1(W0, NAME, Notification, PARAMS, std::nullptr_t)
#define DECL_NOTIFIC_2(W0, W1, NAME, PARAMS) DECL_METHOD_2(W0, W1, NAME, Notification, PARAMS, std::nullptr_t)

// NOLINTBEGIN(readability-identifier-naming)

namespace methods {

DECL_REQUEST_0(initialize, lsp::InitializeParams, lsp::InitializeResult);
DECL_NOTIFIC_0(initialized, rpc::Empty);
DECL_REQUEST_0(shutdown, rpc::Empty, std::nullptr_t);
DECL_NOTIFIC_0(exit, rpc::Empty);

// textDocument
DECL_NOTIFIC_1(textDocument, didOpen, lsp::DidOpenTextDocumentParams);
DECL_NOTIFIC_1(textDocument, didChange, lsp::DidChangeTextDocumentParams);
DECL_REQUEST_1(textDocument, diagnostic, lsp::DocumentDiagnosticParams, lsp::DocumentDiagnosticReport);
DECL_REQUEST_1(textDocument, definition, lsp::DefinitionParams, lsp::DefinitionResult);
DECL_REQUEST_1(textDocument, codeAction, lsp::CodeActionParams,
               int);  // std::variant<lsp::Command, std::vector<lsp::CodeAction>, std::nullptr_t>

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
