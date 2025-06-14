#include <print>

#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
void methods::textDocument::didOpen::operator()(VanadiumLsContext& ctx, const lsp::DidOpenTextDocumentParams& params) {
  ctx->program.Commit([&](auto& modify) {
    modify.update(std::string(params.textDocument.uri), [&](lib::Arena& arena) -> std::string_view {
      auto buf = arena.AllocStringBuffer(params.textDocument.text.size());
      std::println(stderr, "PARSED:\n'''\n{}\n'''", params.textDocument.text);
      std::ranges::copy(params.textDocument.text, buf.begin());
      return {buf.data(), buf.size()};
    });
  });
}
}  // namespace vanadium::ls
