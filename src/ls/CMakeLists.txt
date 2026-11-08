add_library(vanadium_ls STATIC
  src/LanguageServer.cpp
  src/LanguageServerContext.cpp
  src/LanguageServerSolution.cpp
  src/LanguageServerTools.cpp
  src/LanguageServerToolsOptions.cpp
  src/LanguageServerClientMessaging.cpp
  src/StatusReporter.cpp
  #
  src/actions/AddMissingArguments.cpp
  src/actions/UnresolvedSymbolLookup.cpp
  src/actions/LintAutofix.cpp
  #
  src/aux/ImportInspector.cpp
  #
  src/detail/CodeAction.cpp
  src/detail/Completion.cpp
  src/detail/Definition.cpp
  src/detail/Diagnostic.cpp
  src/detail/Helpers.cpp
  src/detail/InlayHint.cpp
  src/detail/References.cpp
  #
  src/methods/_dollar/cancelRequest.cpp
  src/methods/_dollar/setTrace.cpp
  src/methods/completionItem/resolve.cpp
  src/methods/inlayHint/resolve.cpp
  src/methods/textDocument/codeAction.cpp
  src/methods/textDocument/completion.cpp
  src/methods/textDocument/definition.cpp
  src/methods/textDocument/diagnostic.cpp
  src/methods/textDocument/didChange.cpp
  src/methods/textDocument/didClose.cpp
  src/methods/textDocument/didOpen.cpp
  src/methods/textDocument/didSave.cpp
  src/methods/textDocument/documentHighlight.cpp
  src/methods/textDocument/documentSymbol.cpp
  src/methods/textDocument/formatting.cpp
  src/methods/textDocument/hover.cpp
  src/methods/textDocument/inlayHint.cpp
  src/methods/textDocument/references.cpp
  src/methods/textDocument/rename.cpp
  src/methods/textDocument/semanticTokens/range.cpp
  src/methods/textDocument/signatureHelp.cpp
  src/methods/textDocument/typeDefinition.cpp
  src/methods/vanadiumd/buildImportTree.cpp
  src/methods/vanadiumd/dumpTrace.cpp
  src/methods/workspace/didChangeWatchedFiles.cpp
  src/methods/exit.cpp
  src/methods/initialize.cpp
  src/methods/initialized.cpp
  src/methods/shutdown.cpp
)

target_include_directories(vanadium_ls PUBLIC
  interface
)
target_include_directories(vanadium_ls PRIVATE
  include
)

target_link_libraries(vanadium_ls PRIVATE
  vanadium_lib_common
  vanadium_lib_concurrency
  vanadium_lib_lsprotocol
  vanadium_lib_lserver
  vanadium_lib_jsonrpc
  vanadium_lib_trace
  vanadium_core
  vanadium_tooling
  vanadium_lint
  vanadium_format

  glaze::glaze
  magic_enum
)

add_gtest_executable(vanadium_ls)
