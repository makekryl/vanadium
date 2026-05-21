#include <cassert>
#include <format>

#include <vanadium/ast/utils/ASTUtils.h>
#include <vanadium/core/Semantic.h>
#include <vanadium/core/TypeChecker.h>

#include "vanadium/ast/ASTNodes.h"
#include "vanadium/compiler/Codegen.h"
#include "vanadium/lib/Assert.h"

namespace vanadium::compiler::names {

// TODO: optimize like Binder::ShadowMemberKey

namespace {
std::string TypeSymName(TypeSymbol ts) {
  if (ts->Flags() & core::semantic::SymbolFlags::kBuiltin) {
    return std::string(ts->GetName()) + (ts.is_template ? "_template" : "");
  }

  const auto* sf = ast::utils::SourceFileOf(ts->Declaration());
  if (ts->Flags() & core::semantic::SymbolFlags::kAnonymous) {
    auto buf = std::string{sf->module->name};
    const auto push_anonymous_name_component = [&buf, &sf](this auto&& self, const ast::Node* n) -> void {
      const auto* field = n->parent->As<ast::nodes::Field>();
      switch (field->parent->nkind) {
        case ast::NodeKind::StructSpec:
          self(field->parent);
          break;
        case ast::NodeKind::StructTypeDecl:
          buf += "_";
          buf += sf->Text(*field->parent->As<ast::nodes::StructTypeDecl>()->name);
          break;
        default:
          // TODO: ListSpec
          assert(false);
      }
      buf += "_";
      buf += sf->Text(*field->name);
    };
    push_anonymous_name_component(ts->Declaration());
    if (ts.is_template) {
      buf += "_template";
    }
    return buf;
  }

  return std::format("{}_{}{}", sf->module->name, ts->GetName(), ts.is_template ? "_template" : "");
}
}  // namespace

std::string Func(const core::semantic::Symbol* sym) {
  if (sym->Flags() & core::semantic::SymbolFlags::kBuiltin) {
    return std::format("vrt_{}", sym->GetName());
  }
  const auto* sf = ast::utils::SourceFileOf(sym->Declaration());
  return std::format("{}_{}", sf->module->name, sym->GetName());
}

std::string Type(TypeSymbol ts) {
  // TODO: reconsider
  if (ts->Flags() & core::semantic::SymbolFlags::kBuiltin) {
    return std::format("vrt_{}", TypeSymName(ts));
  }
  return TypeSymName(ts);
}

std::string Ctor(TypeSymbol ts) {
  return std::format("{}_ctor", TypeSymName(ts));
}

std::string Dtor(TypeSymbol ts) {
  return std::format("{}_dtor", TypeSymName(ts));
}

std::string CopyCtor(TypeSymbol ts) {
  return std::format("copy_{}", TypeSymName(ts));
}

std::string TInfo(TypeSymbol ts) {
  return std::format("{}_typeinfo", TypeSymName(ts));
}

std::string Getter(TypeSymbol holder, std::string_view member) {
  return std::format("{}_get_{}", TypeSymName(holder), member);
}

std::string Muttor(TypeSymbol holder, std::string_view member) {
  if (!(holder->Flags() & core::semantic::SymbolFlags::kUnion)) {
    return Getter(holder, member);
  }
  return std::format("{}_mut_{}", TypeSymName(holder), member);
}

std::string TplValCtor(TypeSymbol ts) {
  assert(ts.is_template);
  return std::format("{}_tplval_ctor", TypeSymName(ts));
}

std::string TplValDtor(TypeSymbol ts) {
  assert(ts.is_template);
  return std::format("{}_tplval_dtor", TypeSymName(ts));
}

}  // namespace vanadium::compiler::names
