#include "LanguageServerContext.h"
#include "LanguageServerMethods.h"

namespace vanadium::ls {
template <>
void methods::textDocument::didChange::operator()(VanadiumLsContext& ctx,
                                                  const lsp::DidChangeTextDocumentParams& params) {
  ctx->program.Commit([&](auto& modify) {
    modify.update(std::string(params.textDocument.uri), [&](lib::Arena& arena) -> std::string_view {
      const auto new_text = std::get<lsp::TextDocumentContentChangeWholeDocument>(params.contentChanges[0]);

      auto buf = arena.AllocStringBuffer(new_text.text.size());
      std::ranges::copy(new_text.text, buf.begin());
      return {buf.data(), buf.size()};
    });
  });
}
}  // namespace vanadium::ls
