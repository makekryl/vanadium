#pragma once

#include <vanadium/asn1/ast/Asn1cAstWrapper.h>
#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTTypes.h>
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
  ttcn3_ast::LineMapping lines;

  lib::Arena arena;
  std::optional<Asn1cAstWrapper> ast;
  std::vector<Asn1cSyntaxError> errors;

  bool dirty;
};

class Asn1ModuleBasket {
 public:
  template <typename TKey>
  void Update(TKey* key, std::string_view src) {
    UpdateImpl(reinterpret_cast<OpaqueKey*>(key), src);
  }

  template <typename TKey>
  ttcn3_ast::AST Transform(TKey* key, lib::Arena& arena) {
    return TransformImpl(key, arena);
  }

  template <typename TKey>
  auto Keys() const {
    return items_ | std::views::keys | std::views::transform([&](OpaqueKey* k) {
             return reinterpret_cast<TKey*>(k);
           });
  }

  template <typename TKey>
  auto DirtyKeys() const {
    return items_ | std::views::filter([&](auto& p) {
             return p.second.dirty;
           }) |
           std::views::keys | std::views::transform([&](OpaqueKey* k) {
             return reinterpret_cast<TKey*>(k);
           });
  }

 private:
  using OpaqueKey = void;

  void UpdateImpl(OpaqueKey* key, std::string_view src);
  ttcn3_ast::AST TransformImpl(OpaqueKey* key, lib::Arena& arena);

  Asn1ModuleBasketItem* FindModule(std::string_view name);
  ttcn3_ast::AST TransformAST(Asn1ModuleBasketItem& item, lib::Arena& arena);

  std::unordered_map<OpaqueKey*, Asn1ModuleBasketItem> items_;
  std::unordered_map<std::string_view, Asn1ModuleBasketItem*> modules_;
  std::vector<Asn1ModuleBasket*> references_;
};

}  // namespace vanadium::asn1::ast
