#pragma once

#include <vanadium/asn1/ast/Asn1cAstWrapper.h>
#include <vanadium/ast/AST.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/FunctionRef.h>

#include <ranges>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace vanadium::asn1::ast {

// NOLINTNEXTLINE(readability-identifier-naming)
namespace ttcn3_ast = vanadium::ast;

struct Asn1ModuleBasketItem {
  std::string_view src;
  bool dirty;

  lib::Arena arena;
  std::optional<Asn1cAstWrapper> ast;
  std::vector<Asn1cSyntaxError> errors;
};

class Asn1ModuleBasket {
 public:
  template <typename TKey>
  void Update(TKey* key, std::string_view src) {
    UpdateImpl(reinterpret_cast<void*>(key), src);
  }

  template <typename TKey>
  void RefreshTargetAST(std::invocable<TKey*, ttcn3_ast::AST> auto accept) {
    RefreshTargetASTImpl([&](void* key, ttcn3_ast::AST ast) {
      accept(reinterpret_cast<TKey*>(key), std::move(ast));
    });
  }

 private:
  void UpdateImpl(void* key, std::string_view src);
  void RefreshTargetASTImpl(lib::FunctionRef<void(void*, ttcn3_ast::AST)> accept);

  Asn1ModuleBasketItem* FindModule(std::string_view name);
  ttcn3_ast::AST TransformAST(Asn1ModuleBasketItem& item);

  std::unordered_map<void*, Asn1ModuleBasketItem> items_;
  std::unordered_map<std::string_view, Asn1ModuleBasketItem*> modules_;
  std::vector<Asn1ModuleBasket*> references_;
};

}  // namespace vanadium::asn1::ast
