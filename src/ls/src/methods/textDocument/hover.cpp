#include <LSProtocol.h>
#include <LSProtocolEx.h>
#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTNodes.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Program.h>
#include <vanadium/core/Semantic.h>

#include <format>
#include <iterator>
#include <magic_enum/magic_enum.hpp>
#include <ranges>

#include "vanadium/ls/LanguageServerContext.h"
#include "vanadium/ls/LanguageServerConv.h"
#include "vanadium/ls/LanguageServerMethods.h"
#include "vanadium/ls/LanguageServerSession.h"
#include "vanadium/ls/detail/Definition.h"

namespace vanadium::ls {

namespace {
template <typename SubtitleWriterFn>  // todo: reorganize
class MarkdownHoverCardBuilder {
 public:
  MarkdownHoverCardBuilder(SubtitleWriterFn write_subtitle) : write_subtitle_(std::move(write_subtitle)) {
    buf_.reserve(512);  // TODO: check if this is sufficient, maybe pool the buffer
  }

  void WriteHeader(std::string_view typetype, std::string_view name, std::string_view suffix = "") {
    std::format_to(std::back_inserter(buf_), "### {} `{}` {}\n", typetype, name, suffix);
    write_subtitle_(*this);
    WriteSeparator();
  }

  void WriteCodeBlock(std::string_view code) {
    if (code.size() > 1024) {
      return;
    }
    std::format_to(std::back_inserter(buf_), "```ttcn\n{}\n```\n", code);
  }

  void WriteText(std::string_view text) {
    buf_ += text;
    buf_ += "\n";
  }

  void WriteSeparator() {
    buf_ += "\n---\n\n";
  }

  void WithWriter(auto f) {
    f(std::back_inserter(buf_));
    buf_ += "\n";
  }

  std::string&& Build() {
    return std::move(buf_);
  }

 private:
  SubtitleWriterFn write_subtitle_;
  std::string buf_;
};

template <ast::IsNode TParamDescriptorNode>
void WriteMarkdownParameterList(std::back_insert_iterator<std::string> w, const ast::AST& ast,
                                std::span<const TParamDescriptorNode* const> params) {
  const auto last_idx = std::ssize(params) - 1;
  for (const auto& [idx, param] : params | std::views::enumerate) {
    std::format_to(w, "- `");

    // TODO: cleanup builder from this
    if constexpr (std::is_same_v<TParamDescriptorNode, ast::nodes::Field>) {
      if (param->optional) {
        std::format_to(w, "(optional) ");
      }
    }
    if constexpr (std::is_same_v<TParamDescriptorNode, ast::nodes::FormalPar>) {
      if (param->value) {
        std::format_to(w, "(optional) ");
      }
    }

    if constexpr (std::is_same_v<TParamDescriptorNode, ast::nodes::FormalPar>) {
      if (param->direction) {
        std::format_to(w, "{} ", ast.Text(param->direction));
      }
      if (param->restriction) {
        std::format_to(w, "{} ", ast.Text(param->restriction));
      }
    }

    std::format_to(w, "{} {}`", ast.Text(param->type), ast.Text(*param->name));

    if (idx != last_idx) [[likely]] {
      std::format_to(w, "\n");
    }
  }
}

bool WriteAttachedComment(std::back_insert_iterator<std::string> w, const ast::AST& ast, const ast::Node* decl) {
  const auto* tgt_decl = decl;
  if (tgt_decl->nkind == ast::NodeKind::Declarator) {
    tgt_decl = tgt_decl->parent;
  }

  const auto comment = ast::utils::ExtractAttachedComment(ast, tgt_decl);
  if (!comment) {
    return false;
  }

  bool is_inline_comment{false};
  auto comment_sv = ast.Text(*comment);
  if (comment_sv.starts_with("/**")) {
    comment_sv.remove_prefix(3);
  } else if (comment_sv.starts_with("/*")) {
    comment_sv.remove_prefix(2);
  } else if (comment_sv.starts_with("//")) {
    is_inline_comment = true;
    comment_sv.remove_prefix(2);
  }
  while (comment_sv.starts_with("\n") || comment_sv.starts_with(" ")) {
    comment_sv.remove_prefix(1);
  }
  while (comment_sv.ends_with("\n") || comment_sv.ends_with(" ")) {
    comment_sv.remove_suffix(1);
  }
  if (comment_sv.ends_with("**/")) {
    comment_sv.remove_suffix(3);
  } else if (comment_sv.ends_with("*/")) {
    comment_sv.remove_suffix(2);
  }
  while (comment_sv.ends_with("\n") || comment_sv.ends_with(" ")) {
    comment_sv.remove_suffix(1);
  }

  if (is_inline_comment) {
    auto lines = std::views::split(comment_sv, "\n//");
    auto it = lines.begin();
    const auto end_it = lines.end();
    for (; it != end_it; ++it) {
      auto v = std::string_view{*it};
      std::format_to(w, "{}", v);
      if (std::next(it) != end_it) {
        std::format_to(w, "\n");
      }
    }
  } else {
    std::format_to(w, "{}", comment_sv);
  }

  return true;
}

lsp::HoverResult ProvideHover(const lsp::HoverParams& params, const core::SourceFile& file, LsSessionRef d) {
  const auto symres = detail::FindSymbol(&file, params.position);
  if (!symres) {
    return nullptr;
  }
  const auto* cursor_node = symres->node;
  const auto* sym = symres->type.sym;

  if (!sym) {
    return nullptr;
  }
  const auto* decl = sym->Declaration();
  if (!decl) {
    return nullptr;
  }
  const auto* provider_file = ast::utils::SourceFileOf(decl);

  MarkdownHoverCardBuilder builder([&](auto& bld) {
    if (provider_file != &file) {
      if (provider_file->module->name == "LanguageBuiltins") {
        bld.WriteText("_TTCN-3 Standard Definition_");
      } else {
        const auto source_loc = conv::ToLSPPosition(provider_file->ast.lines.Translate(decl->nrange.begin));
        bld.WithWriter([&](auto w) {
          std::format_to(w, "[module {}]({}#L{}C{})", provider_file->module->name,
                         PathToFileUri(d.solution, provider_file->path), source_loc.line + 1, source_loc.character);
        });
      }
    }
  });
  if (decl) {
    bool had_comment{false};
    builder.WithWriter([&](auto w) {
      had_comment = WriteAttachedComment(w, provider_file->ast, decl);
    });
    if (had_comment) {
      builder.WriteSeparator();
    }
  }
  switch (decl->nkind) {
    case ast::NodeKind::FuncDecl: {
      const auto* m = decl->As<ast::nodes::FuncDecl>();
      builder.WriteHeader(provider_file->Text(m->kind.range), provider_file->Text(*m->name));
      builder.WithWriter([&](auto w) {
        std::format_to(w, "→ `{}`", m->ret ? provider_file->ast.Text(m->ret->type) : "void");
        if (m->params && !m->params->list.empty()) {
          std::format_to(w, "\n\nArguments:\n");
          WriteMarkdownParameterList<ast::nodes::FormalPar>(w, provider_file->ast, m->params->list);
        }
      });
      builder.WriteSeparator();
      builder.WriteCodeBlock(provider_file->ast.Text(ast::Range{
          .begin = m->nrange.begin,
          .end = m->body ? m->body->nrange.begin : m->nrange.end,
      }));
      break;
    }
    case ast::NodeKind::TemplateDecl: {
      const auto* m = decl->As<ast::nodes::TemplateDecl>();
      builder.WriteHeader("template", provider_file->Text(*m->name));
      builder.WithWriter([&](auto w) {
        if (m->params && !m->params->list.empty()) {
          std::format_to(w, "Parameters:\n");
          WriteMarkdownParameterList<ast::nodes::FormalPar>(w, provider_file->ast, m->params->list);
        }
      });
      builder.WriteSeparator();
      builder.WriteCodeBlock(provider_file->ast.Text(ast::Range{
          .begin = decl->nrange.begin,
          .end = m->value->nrange.begin,
      }));
      break;
    }
    case ast::NodeKind::ConstructorDecl: {
      const auto* m = decl->As<ast::nodes::ConstructorDecl>();
      builder.WriteHeader("class", provider_file->Text(*decl->parent->parent->As<ast::nodes::ClassTypeDecl>()->name),
                          "constructor");
      builder.WithWriter([&](auto w) {
        if (!m->params->list.empty()) {
          std::format_to(w, "Arguments:\n");
          WriteMarkdownParameterList<ast::nodes::FormalPar>(w, provider_file->ast, m->params->list);
        }
      });
      builder.WriteSeparator();
      builder.WriteCodeBlock(provider_file->ast.Text(ast::Range{
          .begin = m->nrange.begin,
          .end = m->body ? m->body->nrange.begin : m->nrange.end,
      }));
      break;
    }
    case ast::NodeKind::StructTypeDecl: {
      const auto* m = decl->As<ast::nodes::StructTypeDecl>();
      builder.WriteHeader(provider_file->Text(m->kind.range), provider_file->ast.Text(*m->name));
      builder.WithWriter([&](auto w) {
        if (!m->fields.empty()) {
          std::format_to(w, "Fields:\n");
          WriteMarkdownParameterList<ast::nodes::Field>(w, provider_file->ast, m->fields);
        }
      });
      builder.WriteSeparator();
      builder.WriteCodeBlock(provider_file->ast.Text(ast::Range{
          .begin = m->nrange.begin,
          .end = m->name->nrange.end,
      }));
      break;
    }
    case ast::NodeKind::StructSpec: {
      const auto* m = decl->As<ast::nodes::StructSpec>();
      builder.WriteHeader(provider_file->Text(m->kind.range),
                          provider_file->ast.Text(*m->parent->As<ast::nodes::Field>()->name));
      builder.WithWriter([&](auto w) {
        if (!m->fields.empty()) {
          std::format_to(w, "Fields:\n");
          WriteMarkdownParameterList<ast::nodes::Field>(w, provider_file->ast, m->fields);
        }
      });
      builder.WriteSeparator();
      builder.WriteCodeBlock(provider_file->ast.Text(m->parent));
      break;
    }
    case ast::NodeKind::ListSpec: {
      const auto* m = decl->As<ast::nodes::ListSpec>();
      builder.WriteHeader(provider_file->Text(m->kind.range),
                          provider_file->ast.Text(*m->parent->As<ast::nodes::Field>()->name));
      builder.WriteSeparator();
      builder.WriteCodeBlock(provider_file->ast.Text(m->parent));
      break;
    }
    case ast::NodeKind::EnumSpec: {
      const auto* m = decl->As<ast::nodes::EnumSpec>();
      builder.WriteHeader("enum", provider_file->ast.Text(*m->parent->As<ast::nodes::Field>()->name));
      builder.WithWriter([&](auto w) {
        std::format_to(w, "Values:\n");
        const auto last_idx = std::ssize(m->values) - 1;
        for (const auto& [idx, v] : m->values | std::views::enumerate) {
          std::format_to(w, "- `{}`", provider_file->Text(v));
          if (idx != last_idx) [[likely]] {
            std::format_to(w, "\n");
          }
        }
      });
      builder.WriteSeparator();
      builder.WriteCodeBlock(provider_file->ast.Text(m->parent));
      break;
    }
    case ast::NodeKind::EnumTypeDecl: {
      const auto* m = decl->As<ast::nodes::EnumTypeDecl>();
      builder.WriteHeader("enum", provider_file->ast.Text(*m->name));
      builder.WithWriter([&](auto w) {
        std::format_to(w, "Values:\n");
        const auto last_idx = std::ssize(m->values) - 1;
        for (const auto& [idx, v] : m->values | std::views::enumerate) {
          std::format_to(w, "- `{}`", provider_file->Text(v));
          if (idx != last_idx) [[likely]] {
            std::format_to(w, "\n");
          }
        }
      });
      builder.WriteSeparator();
      builder.WriteCodeBlock(provider_file->ast.Text(ast::Range{
          .begin = m->nrange.begin,
          .end = m->name->nrange.end,
      }));
      break;
    }
    case ast::NodeKind::ClassTypeDecl: {
      const auto* m = decl->As<ast::nodes::ClassTypeDecl>();
      builder.WriteHeader("class", provider_file->ast.Text(*m->name));
      builder.WriteCodeBlock(provider_file->ast.Text(ast::Range{
          .begin = m->nrange.begin,
          .end = m->name->nrange.end,
      }));
      break;
    }
    case ast::NodeKind::SubTypeDecl: {
      const auto* m = decl->As<ast::nodes::SubTypeDecl>();
      builder.WriteHeader("subtype", provider_file->ast.Text(*m->field->name));
      builder.WriteCodeBlock(provider_file->ast.Text(ast::Range{
          .begin = m->nrange.begin,
          .end = m->nrange.end,
      }));
      break;
    }
    case ast::NodeKind::Declarator: {
      const auto* m = decl->As<ast::nodes::Declarator>();
      const auto* vd = m->parent->As<ast::nodes::ValueDecl>();

      builder.WriteHeader(vd->kind ? provider_file->ast.Text(vd->kind->range) : "", provider_file->ast.Text(*m->name));
      builder.WithWriter([&](auto w) {
        std::format_to(w, "Type: `{}`", provider_file->ast.Text(vd->type));
      });
      builder.WriteSeparator();
      builder.WriteCodeBlock(provider_file->ast.Text(ast::Range{
          .begin = m->parent->nrange.begin,
          .end = m->value ? m->value->nrange.end : m->name->nrange.end,
      }));
      break;
    }
    case ast::NodeKind::FormalPar: {
      const auto* m = decl->As<ast::nodes::FormalPar>();
      builder.WriteHeader("param", provider_file->ast.Text(*m->name));
      builder.WithWriter([&](auto w) {
        std::format_to(w, "Type: `{}`", provider_file->ast.Text(m->type));
      });
      builder.WriteSeparator();
      builder.WriteCodeBlock(provider_file->ast.Text(ast::Range{
          .begin = m->nrange.begin,
          .end = m->name->nrange.end,
      }));
      break;
    }
    case ast::NodeKind::Field: {
      const auto* m = decl->As<ast::nodes::Field>();

      std::string fh;
      builder.WriteHeader("field", [&] -> std::string_view {
        if (m->parent->nkind == ast::NodeKind::StructTypeDecl) {
          return fh = std::format("{}::{}", provider_file->ast.Text(*m->parent->As<ast::nodes::StructTypeDecl>()->name),
                                  provider_file->ast.Text(*m->name));
        }
        return provider_file->ast.Text(*m->name);
      }());
      builder.WithWriter([&](auto w) {
        std::format_to(w, "Type: `{}` / `{}`", provider_file->ast.Text(m->type), sym->GetName());
      });
      builder.WriteSeparator();
      builder.WriteCodeBlock(provider_file->ast.Text(m));
      break;
    }
    case ast::NodeKind::ImportDecl: {
      const auto* m = decl->As<ast::nodes::ImportDecl>();

      if (cursor_node->parent->nkind == ast::NodeKind::ImportDecl) {
        m = cursor_node->parent->As<ast::nodes::ImportDecl>();
      }

      const auto* module = file.program->GetModule(provider_file->ast.Text(*m->module));
      if (!module) {
        break;
      }

      builder.WriteHeader("module", provider_file->ast.Text(*m->module));
      builder.WithWriter([&](auto w) {
        std::format_to(w, "`{}`", module->sf->path);
        if (!m->list.empty() && m->list[0]->kind.kind == ast::TokenKind::IMPORT) {
          std::format_to(w, "\n");
          builder.WriteSeparator();

          std::format_to(w, "Transitively imports modules:\n");
          for (const auto& [import, descriptor] : module->imports) {
            if (descriptor.transit) {
              std::format_to(w, "- `{}`\n", import);
            }
          }
        }
      });
      break;
    }
    case ast::NodeKind::ComponentTypeDecl: {
      const auto* m = decl->As<ast::nodes::ComponentTypeDecl>();
      builder.WriteHeader("component", provider_file->ast.Text(*m->name));
      builder.WriteCodeBlock(provider_file->ast.Text(ast::Range{
          .begin = m->nrange.begin,
          .end = m->nrange.end,
      }));
      break;
    }
    case ast::NodeKind::PortTypeDecl: {
      const auto* m = decl->As<ast::nodes::PortTypeDecl>();
      builder.WriteHeader("port", provider_file->ast.Text(*m->name));
      builder.WriteCodeBlock(provider_file->ast.Text(ast::Range{
          .begin = m->nrange.begin,
          .end = m->nrange.end,
      }));
      break;
    }
    default:
      if (sym->Flags() & core::semantic::SymbolFlags::kEnumMember) {
        std::string_view enum_name;
        const auto* parent = decl->parent;
        if (parent->nkind == ast::NodeKind::CallExpr) {
          parent = parent->parent;
        }
        switch (parent->nkind) {
          case ast::NodeKind::EnumTypeDecl: {
            enum_name = provider_file->Text(*parent->As<ast::nodes::EnumTypeDecl>()->name);
            break;
          }
          case ast::NodeKind::EnumSpec: {
            auto& buf = *d.arena.Alloc<std::string>();
            for (const auto* pn = parent->parent; pn->nkind == ast::NodeKind::Field;) {
              buf = std::format("{}.{}", provider_file->Text(*pn->As<ast::nodes::Field>()->name), buf);

              if (pn->parent->nkind == ast::NodeKind::StructTypeDecl) {
                buf =
                    std::format("{}.{}", provider_file->Text(*pn->parent->As<ast::nodes::StructTypeDecl>()->name), buf);
              }

              pn = pn->parent->parent;
            }
            enum_name = buf;
            enum_name.remove_suffix(1);  // trailing dot
            break;
          }
          default: {
            enum_name = "<anonymous>";
            break;
          }
        }

        builder.WriteHeader("enum value", enum_name);
        builder.WriteCodeBlock(provider_file->ast.Text(decl));

        break;
      }
      builder.WriteText(std::format("TODO: provide hover details for '{}'", magic_enum::enum_name(decl->nkind)));
      break;
  }

  return lsp::Hover{
      .contents =
          lsp::MarkupContent{
              .kind = lsp::MarkupKind::kMarkdown,
              .value = *d.arena.Alloc<std::string>(std::move(builder.Build())),
          },
      .range = conv::ToLSPRange(cursor_node->nrange, file.ast),
  };
}
}  // namespace

rpc::ExpectedResult<lsp::HoverResult> methods::textDocument::hover::invoke(LsContext& ctx,
                                                                           const lsp::HoverParams& params) {
  return ctx.WithFile<lsp::HoverResult>(params, ProvideHover).value_or(nullptr);
}
}  // namespace vanadium::ls
