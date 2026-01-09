#include "vanadium/asn1/ast/Asn1ModuleBasket.h"

#include <oneapi/tbb/parallel_for_each.h>
#include <vanadium/asn1/ast/Asn1cAstWrapper.h>
#include <vanadium/ast/ASTTypes.h>
#include <vanadium/lib/Arena.h>
#include <vanadium/lib/FunctionRef.h>

#include <mutex>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace vanadium::asn1::ast {

namespace {
std::vector<pos_t> CollectLineStarts(std::string_view src) {
  std::vector<pos_t> line_starts;
  line_starts.clear();
  for (std::size_t i = 0; i < src.size(); ++i) {
    if (src[i] == '\n') {
      line_starts.push_back(i + 1);
    }
  }
  return line_starts;
}
}  // namespace

void Asn1ModuleBasket::UpdateImpl(OpaqueKey* key, std::string_view src) {
  static std::mutex mu;
  std::lock_guard l(mu);

  auto [it, inserted] = items_.try_emplace(key);
  auto& item = it->second;

  if (!inserted) {
    item.arena.Reset();
  }

  item.src = src;
  item.lines = ttcn3_ast::LineMapping(CollectLineStarts(src));
  if (auto result = Parse(item.arena, src)) {
    item.ast = std::move(*result);
    item.errors.clear();
  } else {
    item.ast = std::nullopt;
    item.errors = std::move(result.error());
  }
  item.dirty = true;
}

void Asn1ModuleBasket::RefreshTargetASTImpl(const Refresher<OpaqueKey>& refresher) {
  tbb::parallel_for_each(items_, [&](auto& entry) {
    auto& [key, item] = entry;

    if (!item.dirty) {
      return;
    }

    refresher.accept(key, TransformAST(item, refresher.provide_arena(key)));
  });
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

}  // namespace vanadium::asn1::ast
