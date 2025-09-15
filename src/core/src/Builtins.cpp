#include "Builtins.h"

#include "ASTNodes.h"
#include "TypeChecker.h"

#ifndef NDEBUG
#include <print>
#endif

#include "AST.h"
#include "Arena.h"
#include "Parser.h"
#include "Program.h"
#include "Semantic.h"
#include "StaticMap.h"

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
const semantic::Symbol kTimer{
    "timer", nullptr,
    semantic::SymbolFlags::Value(semantic::SymbolFlags::kBuiltinType | semantic::SymbolFlags::kClassType),
    [] -> semantic::Scope* {
      static semantic::Scope scope(nullptr);
      scope.symbols.Add(semantic::Symbol("timeout", nullptr, semantic::SymbolFlags::kBuiltin));
      // scope.symbols.Add(semantic::Symbol(
      //     "start", [] -> const ast::nodes::FuncDecl* {

      //     }(),
      //     semantic::SymbolFlags::Value(semantic::SymbolFlags::kFunction | semantic::SymbolFlags::kBuiltin)));
      return &scope;
    }()};

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
      {"timer", &kTimer},

      {"object", &kAnytype},  // TODO

      // DO NOT USE OR YOU WILL BE FIRED! (c)
      {"__infer_arg_t", &checker::symbols::kInferType},
  });
  if (const auto sym_opt = kBuiltinsTable.get(name); sym_opt) {
    return *sym_opt;
  }
  return nullptr;
}
}  // namespace

namespace {
constexpr std::string_view kBuiltinsModule{
#include "blob/TTCNBuiltinDefs.inc"
};

const semantic::Symbol* ResolveBuiltinDef(std::string_view name) {
  static const auto* scope = [] {
    static lib::Arena arena;

    static SourceFile sf{
        .path = "",
        .ast = ast::Parse(arena, kBuiltinsModule),
    };
    sf.ast.root->file = &sf;
#ifndef NDEBUG
    if (!sf.ast.errors.empty()) {
      std::println(stderr, "Builtin definitions module has syntax errors:");
      for (const auto& err : sf.ast.errors) {
        std::println(stderr, " - ({}:{}) {}", err.range.begin, err.range.end, err.description);
      }
      std::fflush(stderr);
      std::abort();
    }
#endif

    sf.module.emplace();
    sf.module->name = "TTCNBuiltins";
    sf.module->sf = &sf;
    static semantic::Scope scope{sf.ast.root};
    sf.module->scope = &scope;

    sf.ast.root->Accept([&](const ast::Node* n) {
      switch (n->nkind) {
        case ast::NodeKind::FuncDecl: {
          const auto* m = n->As<ast::nodes::FuncDecl>();
          scope.symbols.Add({
              sf.Text(*m->name),
              m,
              semantic::SymbolFlags::Value(semantic::SymbolFlags::kFunction | semantic::SymbolFlags::kBuiltin),
          });
          return false;
        }
        case ast::NodeKind::ValueDecl: {
          const auto* m = n->As<ast::nodes::ValueDecl>();
          for (const auto* d : m->decls) {
            scope.symbols.Add({
                sf.Text(*d->name),
                m,
                semantic::SymbolFlags::Value(semantic::SymbolFlags::kVariable | semantic::SymbolFlags::kBuiltin),
            });
          }
          return false;
        }
        default: {
          return true;
        }
      }
    });

    return &scope;
  }();
  return scope->ResolveDirect(name);
}

}  // namespace

const semantic::Symbol* ResolveBuiltin(std::string_view name) {
  if (const auto* sym = ResolveBuiltinType(name); sym) {
    return sym;
  }
  if (const auto* sym = ResolveBuiltinDef(name); sym) {
    return sym;
  }
  return nullptr;
}

}  // namespace builtins
}  // namespace vanadium::core
