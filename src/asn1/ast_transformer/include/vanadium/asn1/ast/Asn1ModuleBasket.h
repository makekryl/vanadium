#pragma once

#include <vanadium/asn1/ast/Asn1cAstWrapper.h>
#include <vanadium/ast/AST.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/FunctionRef.h>

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
  struct Refresher {
    lib::FunctionRef<void(TKey*)> prepare;
    lib::FunctionRef<lib::Arena&(TKey*)> provide_arena;
    lib::FunctionRef<void(TKey*, ttcn3_ast::AST)> accept;
  };

  template <typename TKey>
  void Update(TKey* key, std::string_view src) {
    UpdateImpl(reinterpret_cast<OpaqueKey*>(key), src);
  }

  template <typename TKey>
  void RefreshTargetAST(const Refresher<TKey>& refresher) {
    RefreshTargetASTImpl(reinterpret_cast<const Refresher<OpaqueKey>&>(refresher));
  }

 private:
  using OpaqueKey = void;

  void UpdateImpl(OpaqueKey* key, std::string_view src);
  void RefreshTargetASTImpl(const Refresher<OpaqueKey>& refresher);

  Asn1ModuleBasketItem* FindModule(std::string_view name);
  ttcn3_ast::AST TransformAST(Asn1ModuleBasketItem& item, lib::Arena& arena);

  std::unordered_map<OpaqueKey*, Asn1ModuleBasketItem> items_;
  std::unordered_map<std::string_view, Asn1ModuleBasketItem*> modules_;
  std::vector<Asn1ModuleBasket*> references_;
};

}  // namespace vanadium::asn1::ast
