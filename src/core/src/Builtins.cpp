#include "Builtins.h"

#include <print>

#include "AST.h"
#include "ASTNodes.h"
#include "Arena.h"
#include "BuiltinsSuperbases.h"
#include "Parser.h"
#include "Program.h"
#include "Semantic.h"
#include "StaticMap.h"
#include "TypeChecker.h"

namespace vanadium::core {
namespace builtins {

const semantic::Symbol kAnytype{"anytype", nullptr, semantic::SymbolFlags::kBuiltinType};
const semantic::Symbol kBoolean{"boolean", nullptr, semantic::SymbolFlags::kBuiltinType};
const semantic::Symbol kInteger{"integer", nullptr, semantic::SymbolFlags::kBuiltinType};
const semantic::Symbol kFloat{"float", nullptr, semantic::SymbolFlags::kBuiltinType};
const semantic::Symbol kBitstring{"bitstring", nullptr, semantic::SymbolFlags::kBuiltinStringType};
const semantic::Symbol kCharstring{"charstring", nullptr, semantic::SymbolFlags::kBuiltinStringType};
const semantic::Symbol kOctetstring{"octetstring", nullptr, semantic::SymbolFlags::kBuiltinStringType};
const semantic::Symbol kHexstring{"hexstring", nullptr, semantic::SymbolFlags::kBuiltinStringType};
const semantic::Symbol kUniversalCharstring{"universal charstring", nullptr, semantic::SymbolFlags::kBuiltinStringType};
const semantic::Symbol kVerdictType{"verdicttype", nullptr, semantic::SymbolFlags::kBuiltinType};

Superbases superbases{};

namespace {
const semantic::Symbol* ResolveBuiltinType(std::string_view name) {
  static constexpr auto kBuiltinsTable = lib::MakeStaticMap<std::string_view, const semantic::Symbol*>({
      {"anytype", &kAnytype},
      {"boolean", &kBoolean},
      {"integer", &kInteger},
      {"float", &kFloat},
      {"bitstring", &kBitstring},
      {"charstring", &kCharstring},
      {"octetstring", &kOctetstring},
      {"hexstring", &kHexstring},
      {"universal charstring", &kCharstring},  // TODO: investigate &kUniversalCharstring
      {"verdicttype", &kVerdictType},

      {"object", &kAnytype},  // TODO

      // DO NOT USE OR YOU WILL BE FIRED! (c)
      {"__infer_arg_t", &checker::symbols::kInferType},
      {"__altstep_t", &checker::symbols::kAltstepType},
  });
  if (const auto sym_opt = kBuiltinsTable.get(name); sym_opt) {
    return *sym_opt;
  }
  return nullptr;
}
}  // namespace

namespace {
constexpr std::string_view kBuiltinsSource{
#include "blob/TTCNBuiltinDefs.inc"
};

const semantic::Scope* const kBuiltinsScope = [] {
  static lib::Arena arena;

  static SourceFile sf{
      .path = "",
      .ast = ast::Parse(arena, kBuiltinsSource),
  };
  sf.ast.root->file = &sf;
  if (!sf.ast.errors.empty()) [[unlikely]] {
    std::println(stderr, "Builtin definitions module has syntax errors:");
    for (const auto& err : sf.ast.errors) {
      std::println(stderr, " - ({}:{}) {}", err.range.begin, err.range.end, err.description);
    }
    std::fflush(stderr);
    std::abort();
  }

  sf.module.emplace();
  sf.module->name = "LanguageBuiltins";
  sf.module->sf = &sf;
  static semantic::Scope root_scope{sf.ast.root};
  sf.module->scope = &root_scope;

  semantic::Scope* scope{&root_scope};
  sf.ast.root->Accept([&](this auto&& self, const ast::Node* n) {
    switch (n->nkind) {
      case ast::NodeKind::FuncDecl: {
        const auto* m = n->As<ast::nodes::FuncDecl>();
        scope->symbols.Add({
            sf.Text(*m->name),
            m,
            semantic::SymbolFlags::Value(semantic::SymbolFlags::kFunction | semantic::SymbolFlags::kBuiltin),
        });
        return false;
      }
      case ast::NodeKind::ValueDecl: {
        const auto* m = n->As<ast::nodes::ValueDecl>();
        for (const auto* d : m->decls) {
          constexpr std::string_view kStripTrailing{"_"};
          auto name = sf.Text(*d->name);
          if (name.ends_with(kStripTrailing)) {
            name.remove_suffix(kStripTrailing.size());
          }
          scope->symbols.Add({
              name,
              d,
              semantic::SymbolFlags::Value(semantic::SymbolFlags::kVariable | semantic::SymbolFlags::kBuiltin),
          });
        }
        return false;
      }
      case ast::NodeKind::SubTypeDecl: {
        const auto* m = n->As<ast::nodes::SubTypeDecl>();
        scope->symbols.Add(semantic::Symbol{
            sf.Text(*m->field->name),
            m,
            semantic::SymbolFlags::kSubTypeType,
        });
        return false;
      }
      case ast::NodeKind::ClassTypeDecl: {
        auto* prev_scope{scope};
        scope = sf.arena.Alloc<semantic::Scope>(n, prev_scope);
        //

        const auto* m = n->As<ast::nodes::ClassTypeDecl>();
        for (const auto* d : m->defs) {
          d->Accept(self);
        }

        // todo switch based on name if timer then push to parent scope otherwise assign the symbol to a variable inside
        // builtins and return it from GetSuperscope
        const auto name = sf.Text(*m->name);
        if (name == "Timer") {
          prev_scope->symbols.Add({
              "timer",
              m,
              semantic::SymbolFlags::Value(semantic::SymbolFlags::kClassType | semantic::SymbolFlags::kBuiltin),
              scope,
          });
        } else if (name == "BuiltinTestcaseType") {
          prev_scope->symbols.Add({
              name,
              m,
              semantic::SymbolFlags::Value(semantic::SymbolFlags::kClassType | semantic::SymbolFlags::kBuiltin |
                                           semantic::SymbolFlags::kAnonymous),
              scope,
          });
        } else {
          const auto* sym = sf.arena.Alloc<semantic::Symbol>(semantic::Symbol{
              name,
              m,
              semantic::SymbolFlags::Value(semantic::SymbolFlags::kClassType | semantic::SymbolFlags::kBuiltin),
              scope,
          });

          if (name == "PortSuperbase") {
            superbases.kPort = sym;
          } else if (name == "ComponentSuperbase") {
            scope->symbols.Add({
                "create",
                m,
                semantic::SymbolFlags::kBuiltin,
            });
            superbases.kComponent = sym;
          } else {
            std::println(stderr, "Builtin definitions module has unknown class: {}", name);
            std::fflush(stderr);
            std::abort();
          }
        }

        //
        scope = prev_scope;
        return false;
      }
      default: {
        return true;
      }
    }
  });

  return &root_scope;
}();

}  // namespace

const semantic::Symbol* ResolveBuiltin(std::string_view name) {
  if (const auto* sym = ResolveBuiltinType(name); sym) {
    return sym;
  }
  if (const auto* sym = kBuiltinsScope->ResolveDirect(name); sym) {
    return sym;
  }
  return nullptr;
}

}  // namespace builtins
}  // namespace vanadium::core
