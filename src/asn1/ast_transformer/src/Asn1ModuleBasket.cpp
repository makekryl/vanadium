#include "vanadium/asn1/ast/Asn1ModuleBasket.h"

#include <asn1c/libasn1parser/asn1parser_cxx.h>
#include <vanadium/asn1/ast/Asn1cAstWrapper.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/FunctionRef.h>

#include <mutex>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "vanadium/asn1/ast/Asn1AstTransformer.h"

namespace vanadium::asn1::ast {

namespace {
std::vector<pos_t> CollectLineStarts(std::string_view src) {
  std::vector<pos_t> line_starts;
  for (std::size_t i = 0; i < src.size(); ++i) {
    if (src[i] == '\n') {
      line_starts.push_back(i + 1);
    }
  }
  return line_starts;
}
}  // namespace

void Asn1ModuleBasket::AddReference(Asn1ModuleBasket* ref) {
  references_.emplace_back(ref);
}

void Asn1ModuleBasket::UpdateImpl(OpaqueKey* key, std::string_view src) {
  static std::mutex mu;  // todo: check global variables in l/y
  std::lock_guard l(mu);

  auto [it, inserted] = items_.try_emplace(key);
  auto& item = it->second;

  if (!inserted) {
    item.arena.Reset();
    if (item.module_name) {
      modules_.erase(*item.module_name);
    }
  }

  item.src = src;
  item.lines = ttcn_ast::LineMapping(CollectLineStarts(src));
  if (auto result = Parse(item.arena, src)) {
    item.ast = std::move(*result);
    item.errors.clear();
    RegisterModule(item);
  } else {
    item.ast = std::nullopt;
    item.errors = std::move(result.error());
  }
}

void Asn1ModuleBasket::RegisterModule(Asn1ModuleBasketItem& item) {
  if (const asn1p_module_t* mod = TQ_FIRST(&(item.ast->Raw()->modules)); mod) {
    item.module_name = mod->ModuleName;

    auto it = modules_.find(*item.module_name);
    if (it == modules_.end()) {
      modules_.emplace(*item.module_name, &item);
      return;
    }

    item.errors.emplace_back(Asn1cSyntaxError{
        .range = {},
        .message = std::format("module '{}' is already defined in another file", *item.module_name),
    });
  }
}

Asn1ModuleBasketItem* Asn1ModuleBasket::FindModuleProvider(std::string_view name) {
  if (auto it = modules_.find(name); it != modules_.end()) {
    return it->second;
  }
  for (auto* ref : references_) {
    if (auto* module = ref->FindModuleProvider(name)) {
      return module;
    }
  }
  return nullptr;
}

ttcn_ast::AST Asn1ModuleBasket::TransformImpl(OpaqueKey* key, lib::Arena& arena) {
  const auto& item = items_.at(key);

  std::vector<ttcn_ast::SyntaxError> errors;
  errors.reserve(item.errors.size());
  //
  for (const auto& err : item.errors) {
    errors.emplace_back(ttcn_ast::SyntaxError{
        .range = {.begin = err.range.begin, .end = err.range.end},
        .description = err.message,
    });
  }

  if (!item.ast) {
    return {
        .src = item.src,
        .root =
            [&] {
              auto* n = arena.Alloc<ttcn_ast::RootNode>();
              n->nrange = {};
              return n;
            }(),
        .lines = item.lines,
        .errors = std::move(errors),
    };
  }

  auto transformed_ast = TransformAsn1Ast(item.ast->Raw(), item.src, arena,
                                          [&](const char* required_module_name) -> const asn1p_module_t* {
                                            const auto* provider = FindModuleProvider(required_module_name);
                                            if (!provider) {
                                              return nullptr;
                                            }
                                            return TQ_FIRST(&(provider->ast->Raw()->modules));
                                          });

  errors.reserve(errors.size() + transformed_ast.errors.size());
  //
  for (auto& err : transformed_ast.errors) {
    errors.emplace_back(ttcn_ast::SyntaxError{
        .range = err.range,
        .description = std::move(err.message),
    });
  }

  return {
      .src = transformed_ast.adjusted_src,
      .root = transformed_ast.root,
      .lines = item.lines,
      .errors = std::move(errors),
  };
}

}  // namespace vanadium::asn1::ast
