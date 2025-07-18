#pragma once

#include <cassert>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "ASTNodes.h"

namespace vanadium::core {

namespace semantic {

struct SemanticError {
  ast::Range range;
  enum class Type : std::uint8_t {
    kRedefinition,
    kRunsOnRequiresComponent,
    kToDo,
  } type;
};

class Scope;
class Symbol;
class SymbolTable;

namespace SymbolFlags {  // NOLINT(readability-identifier-naming)
enum Value : std::uint16_t {
  kNone = 0,

  kImportedName = 1 << 0,

  kVariable = 1 << 1,
  kArgument = 1 << 2,

  kFunction = 1 << 3,
  kTemplate = 1 << 4,

  kComponent = 1 << 5,
  kStructural = 1 << 6,
  kSubType = 1 << 7,
  kEnum = 1 << 8,

  kField = 1 << 9,
  kEnumValue = 1 << 10,

  kArray = 1 << 11,
};
}

class Symbol {
 public:
  Symbol(std::string_view name, const ast::Node* declaration, SymbolFlags::Value flags)
      : name_(name), declaration_(declaration), flags_(flags), containment_(nullptr) {}
  Symbol(std::string_view name, const ast::Node* declaration, SymbolFlags::Value flags, Scope* containment)
      : name_(name), declaration_(declaration), flags_(flags), containment_({.scope = containment}) {}
  Symbol(std::string_view name, const ast::Node* declaration, SymbolFlags::Value flags, SymbolTable* containment)
      : name_(name), declaration_(declaration), flags_(flags), containment_({.members = containment}) {}

  [[nodiscard]] std::string_view GetName() const noexcept {
    return name_;
  };

  [[nodiscard]] const ast::Node* Declaration() const noexcept {
    return declaration_;
  };

  [[nodiscard]] SymbolFlags::Value Flags() const noexcept {
    return flags_;
  }

  [[nodiscard]] const Scope* OriginatedScope() const noexcept {
    return containment_.scope;
  }

  [[nodiscard]] const SymbolTable* Members() const noexcept {
    return containment_.members;
  }

 private:
  std::string_view name_;
  const ast::Node* declaration_;
  SymbolFlags::Value flags_;
  union {
    Scope* scope;
    SymbolTable* members;
  } containment_;
};

class SymbolTable {
 public:
  void Add(Symbol&& symbol) {
    assert(!Has(symbol.GetName()));
    names_.emplace(symbol.GetName(), std::move(symbol));
  }

  bool Has(std::string_view name) const {
    return names_.contains(name);
  }

  [[nodiscard]] std::unordered_map<std::string_view, Symbol> Enumerate() const {
    return names_;
  }

  const Symbol* Lookup(std::string_view name) const {
    auto it = names_.find(name);
    if (it == names_.end()) {
      return nullptr;
    }
    return std::addressof(it->second);
  }

 private:
  std::unordered_map<std::string_view, Symbol> names_;
};

class Scope {
 public:
  Scope(const ast::Node* container, Scope* parent = nullptr) : parent_(parent), container_(container) {}

  const Symbol* Resolve(std::string_view name) const {
    if (const auto* sym = symbols.Lookup(name); sym != nullptr) {
      return sym;
    }

    for (const auto* aug : augmentation) {
      if (const auto* sym = aug->Lookup(name); sym) {
        return sym;
      }
    }

    for (const Scope* scope = parent_; scope != nullptr; scope = scope->parent_) {
      if (const auto* sym = scope->Resolve(name); sym != nullptr) {
        return sym;
      }
    }

    return nullptr;
  }

  const Symbol* ResolveDirect(std::string_view name) const {
    if (const auto* sym = symbols.Lookup(name); sym != nullptr) {
      return sym;
    }

    for (const Scope* scope = parent_; scope != nullptr; scope = scope->parent_) {
      if (const auto* sym = scope->Resolve(name); sym != nullptr) {
        return sym;
      }
    }

    return nullptr;
  }

  const ast::Node* GetContainer() const {
    return container_;
  }

  [[nodiscard]] const std::vector<Scope*>& GetChildren() const {
    return children_;
  }

  void AddChild(Scope* scope) {
    children_.push_back(scope);
  }

 private:
  Scope* parent_;
  std::vector<Scope*> children_;

  const ast::Node* container_;

 public:
  SymbolTable symbols;
  std::vector<const SymbolTable*> augmentation;
};

}  // namespace semantic

}  // namespace vanadium::core
