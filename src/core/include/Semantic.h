#pragma once

#include <cassert>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "ASTNodes.h"
#include "Builtins.h"

namespace vanadium::core {

struct SourceFile;

namespace semantic {

struct SemanticError {
  ast::Range range;
  enum class Type : std::uint8_t {
    kRedefinition,
    kRunsOnRequiresComponent,
    kClassCanBeExtendedByClassOnly,
    kCannotHaveAbstractFunctionInNonAbstractClass,
    kToDo,
  } type;
};

class Scope;
class Symbol;
class SymbolTable;

namespace SymbolFlags {  // NOLINT(readability-identifier-naming)
enum Value : std::uint32_t {
  kNone = 0,

  kImportedModule = 1 << 0,

  kVariable = 1 << 1,
  kArgument = 1 << 2,

  kFunction = 1 << 3,
  kTemplate = 1 << 4,
  kControl = 1 << 5,

  kType = 1 << 6,

  kStructural = 1 << 7,
  kStructuralType = kType | kStructural,

  kComponent = 1 << 8,
  kComponentStructuralType = kType | kStructural | kComponent,

  kUnion = 1 << 9,
  kUnionStructuralType = kType | kStructural | kUnion,

  kClass = 1 << 10,
  kClassType = kType | kClass,

  kConstructor = 1 << 11,
  kConstructorFunction = kFunction | kConstructor,

  kEnum = 1 << 12,
  kEnumType = kType | kEnum,

  kSubType = 1 << 13,
  kSubTypeType = kType | kSubType,

  kThis = 1 << 14,
  kField = 1 << 15,
  kEnumMember = 1 << 16,

  kArray = 1 << 17,

  kBuiltin = 1 << 18,
  kBuiltinType = kType | kBuiltin,
  kBuiltinString = 1 << 19,
  kBuiltinStringType = kType | kBuiltin | kBuiltinString,
  kBuiltinDef = 1 << 20,

  kTemplateSpec = 1 << 21,

  kPort = 1 << 22,

  kVisibilityStatic = 1 << 23,
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
    names_.insert_or_assign(symbol.GetName(), std::move(symbol));
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
    if (const auto* sym = builtins::ResolveBuiltin(name)) {
      return sym;
    }
    return ResolveOwn(name);
  }

  const Symbol* ResolveOwn(std::string_view name) const {
    if (const auto* sym = symbols.Lookup(name); sym != nullptr) {
      return sym;
    }

    for (const auto* aug : augmentation) {
      if (const auto* sym = aug->Lookup(name); sym) {
        return sym;
      }
    }

    for (const Scope* scope = parent_; scope != nullptr; scope = scope->parent_) {
      if (const auto* sym = scope->ResolveOwn(name); sym != nullptr) {
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
      if (const auto* sym = scope->ResolveOwn(name); sym != nullptr) {
        return sym;
      }
    }

    return nullptr;
  }

  const ast::Node* Container() const {
    return container_;
  }

  [[nodiscard]] const std::vector<Scope*>& GetChildren() const {
    return children_;
  }

  void AddChild(Scope* scope) {
    children_.push_back(scope);
  }

  const Scope* ParentScope() const {
    return parent_;
  }

 private:
  Scope* parent_;
  std::vector<Scope*> children_;

  const ast::Node* container_;

 public:
  SymbolTable symbols;
  std::vector<const SymbolTable*> augmentation;
};

void Bind(SourceFile&);

}  // namespace semantic

}  // namespace vanadium::core
