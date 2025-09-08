#pragma once

#include <cassert>
#include <cstdint>
#include <ranges>
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

  kVisibilityStatic = 1 << 7,

  kStructural = 1 << 8,
  kStructuralType = kType | kStructural,

  kComponent = 1 << 9,
  kComponentType = kType | kComponent,

  kUnion = 1 << 10,
  kUnionStructuralType = kType | kStructural | kUnion,

  kClass = 1 << 11,
  kClassType = kType | kClass,

  kConstructor = 1 << 12,
  kConstructorFunction = kFunction | kConstructor | kVisibilityStatic,

  kEnum = 1 << 13,
  kEnumType = kType | kEnum,

  kSubtype = 1 << 14,
  kSubTypeType = kType | kSubtype,

  kList = 1 << 15,
  kListType = kType | kList,

  kThis = 1 << 16,
  kField = 1 << 17,
  kEnumMember = 1 << 18,

  kArray = 1 << 19,

  kBuiltin = 1 << 20,
  kBuiltinType = kType | kBuiltin,
  kBuiltinString = 1 << 21,
  kBuiltinStringType = kType | kBuiltin | kBuiltinString,

  kTemplateSpec = 1 << 22,

  kPort = 1 << 23,
  kPortType = kType | kPort,

  kAnonymous = 1 << 24,
};
}

inline constexpr char kShadowStaticMemberPrefix = '$';
[[nodiscard]] std::string_view ShadowMemberKey(std::string_view opaque_name);

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

  [[nodiscard]] auto Enumerate() const {
    return names_ | std::ranges::views::filter([](const auto& kv) {
             return !(kv.second.Flags() & SymbolFlags::kAnonymous);
           });
  }

  const Symbol* Lookup(std::string_view name) const {
    auto it = names_.find(name);
    if (it == names_.end()) {
      return nullptr;
    }
    return std::addressof(it->second);
  }

  const Symbol* LookupShadow(std::string_view name) const {
    return Lookup(ShadowMemberKey(name));
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

  const Symbol* ResolveHorizontally(std::string_view name) const {
    if (const auto* sym = symbols.Lookup(name); sym != nullptr) {
      return sym;
    }

    for (const auto* aug : augmentation) {
      if (const auto* sym = aug->Lookup(name); sym) {
        return sym;
      }
    }

    return nullptr;
  }

  const Symbol* ResolveOwn(std::string_view name) const {
    if (const auto* sym = ResolveHorizontally(name); sym) {
      return sym;
    }

    for (const Scope* scope = parent_; scope != nullptr; scope = scope->parent_) {
      if (const auto* sym = scope->ResolveOwn(name); sym != nullptr) {
        return sym;
      }
    }

    return nullptr;
  }

  const Symbol* ResolveDirect(std::string_view name) const {
    return symbols.Lookup(name);
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
