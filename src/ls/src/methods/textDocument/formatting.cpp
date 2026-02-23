#include <LSProtocol.h>
#include <LSProtocolEx.h>

#include <vanadium/core/Program.h>
#include <vanadium/format/AstPrinter.h>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerConv.h"
#include "vanadium/ls/LanguageServerMethods.h"
#include "vanadium/ls/LanguageServerSession.h"

namespace vanadium::ls {
namespace {
// https://github.com/prettier/prettier-vscode/blob/ba0c734448feeb7a27a8e48d8c282251141db7b0/src/PrettierEditService.ts#L372C1-L397C4
lsp::TextEdit MinimalEdit(const core::SourceFile& file, std::string_view string1) {
  const auto& string0 = file.ast.src;

  // length of common prefix
  std::size_t i = 0;
  while (i < string0.length() && i < string1.length() && string0[i] == string1[i]) {
    ++i;
  }

  // length of common suffix
  std::size_t j = 0;
  while (i + j < string0.length() && i + j < string1.length() &&
         string0[string0.length() - j - 1] == string1[string1.length() - j - 1]) {
    ++j;
  }

  const auto new_text = string1.substr(i, string1.length() - j - i);
  const auto pos0 = conv::ToLSPPosition(file.ast.lines.Translate(i));
  const auto pos1 = conv::ToLSPPosition(file.ast.lines.Translate(string0.length() - j));

  return lsp::TextEdit{
      .range = lsp::Range{.start = pos0, .end = pos1},
      .newText = new_text,
  };
}

lsp::DocumentFormattingResult Format(const lsp::DocumentFormattingParams& params, const core::SourceFile& file,
                                     LsSessionRef d) {
  format::PrintOptions opts;
  if (d.tools.fmt_opts) {
    opts = *d.tools.fmt_opts;
  } else {
    // TODO: support other options, trailing newline toggle at least
    opts.tab_width = params.options.tabSize;
  }

  const auto* formatted =
      d.arena.Alloc<std::string>(format::PrintAst(file.ast, file.ast.root, d.arena, std::move(opts)));
  return std::vector<lsp::TextEdit>{MinimalEdit(file, *formatted)};
}
}  // namespace

rpc::ExpectedResult<lsp::DocumentFormattingResult> methods::textDocument::formatting::invoke(
    LsContext& ctx, const lsp::DocumentFormattingParams& params) {
  return ctx.WithFile<lsp::DocumentFormattingResult>(params, Format).value_or(nullptr);
}
}  // namespace vanadium::ls
