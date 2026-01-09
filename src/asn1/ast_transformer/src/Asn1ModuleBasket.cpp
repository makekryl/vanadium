#include "vanadium/asn1/ast/Asn1ModuleBasket.h"

#include <vanadium/asn1/ast/Asn1cAstWrapper.h>
#include <vanadium/ast/AST.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/FunctionRef.h>

#include <ranges>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace vanadium::asn1::ast {

void Asn1ModuleBasket::UpdateImpl(void* key, std::string_view src) {
  auto [it, inserted] = items_.try_emplace(key);
  auto& item = it->second;

  if (!inserted) {
    item.arena.Reset();
  }

  item.src = src;
  if (auto result = Asn1cAstWrapper::Parse(item.arena, src)) {
    item.ast = std::move(*result);
    item.errors.clear();
  } else {
    item.ast = std::nullopt;
    item.errors = std::move(result.error());
  }
  item.dirty = true;
}

void Asn1ModuleBasket::RefreshTargetASTImpl(lib::FunctionRef<void(void*, ttcn3_ast::AST)> accept) {
  for (auto& [key, item] : items_) {
    if (!item.dirty) {
      continue;
    }

    accept(key, TransformAST(item));
  }
}

Asn1ModuleBasketItem* Asn1ModuleBasket::FindModule(std::string_view name) {
  if (auto it = modules_.find(name); it != modules_.end()) {
    return it->second;
  }
  for (auto* ref : references_) {
    if (auto* module = ref->FindModule(name)) {
      return module;
    }
  }
  return nullptr;
}

ttcn3_ast::AST Asn1ModuleBasket::TransformAST(Asn1ModuleBasketItem& item) {
  return {
      .src = item.src, .root = nullptr, .lines = {}, .errors = {},
      // .errors = item.errors | std::views::transform([](const auto& err) {
      //             return ttcn3_ast::SyntaxError{
      //                 .range = {.begin = err.pos, .end = err.pos},
      //                 .description = err.message,
      //             };
      //           }) |
      //           std::ranges::to<std::vector>(),
  };
}

}  // namespace vanadium::asn1::ast
