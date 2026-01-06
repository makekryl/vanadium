struct Ident : Node {
  static constexpr NodeKind kKind = NodeKind::Ident;

  Ident() : Node(NodeKind::Ident) {}


  void Accept(const NodeInspector&) const {
  }
};

struct OidComponent : Node {
  static constexpr NodeKind kKind = NodeKind::OidComponent;

  OidComponent() : Node(NodeKind::OidComponent) {}

  Node* content;

  void Accept(const NodeInspector& inspector) const {
    Inspect(content, inspector);
  }
};

struct Assignment : Node {
  static constexpr NodeKind kKind = NodeKind::Assignment;

  Assignment() : Node(NodeKind::Assignment) {}

  Ident name;
  Node* rhs;

  void Accept(const NodeInspector& inspector) const {
    Inspect(&name, inspector);
    Inspect(rhs, inspector);
  }
};

struct Symbol : Node {
  static constexpr NodeKind kKind = NodeKind::Symbol;

  Symbol() : Node(NodeKind::Symbol) {}

  Ident ident;
  bool parametrized{false};

  void Accept(const NodeInspector& inspector) const {
    Inspect(&ident, inspector);
  }
};

struct Value : Node {
  static constexpr NodeKind kKind = NodeKind::Value;

  Value() : Node(NodeKind::Value) {}

  Node* content;

  void Accept(const NodeInspector& inspector) const {
    Inspect(content, inspector);
  }
};

struct Parameter : Node {
  static constexpr NodeKind kKind = NodeKind::Parameter;

  Parameter() : Node(NodeKind::Parameter) {}

  Ident name;
  Node* governor{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(&name, inspector);
    if (governor != nullptr) {
      Inspect(governor, inspector);
    }
  }
};

struct ObjectClass : Node {
  static constexpr NodeKind kKind = NodeKind::ObjectClass;

  ObjectClass() : Node(NodeKind::ObjectClass) {}

  Node* definition;

  void Accept(const NodeInspector& inspector) const {
    Inspect(definition, inspector);
  }
};

struct Tag : Node {
  static constexpr NodeKind kKind = NodeKind::Tag;

  Tag() : Node(NodeKind::Tag) {}

  Token tagClass;
  Token number;
  Token mode;

  void Accept(const NodeInspector&) const {
  }
};

struct BuiltinTypeIdent : Node {
  static constexpr NodeKind kKind = NodeKind::BuiltinTypeIdent;

  BuiltinTypeIdent() : Node(NodeKind::BuiltinTypeIdent) {}

  Token tok1;
  Token tok2;

  void Accept(const NodeInspector&) const {
  }
};

struct ActualParameter : Node {
  static constexpr NodeKind kKind = NodeKind::ActualParameter;

  ActualParameter() : Node(NodeKind::ActualParameter) {}

  Node* value;

  void Accept(const NodeInspector& inspector) const {
    Inspect(value, inspector);
  }
};

struct EnumerationItem : Node {
  static constexpr NodeKind kKind = NodeKind::EnumerationItem;

  EnumerationItem() : Node(NodeKind::EnumerationItem) {}

  Node* value;

  void Accept(const NodeInspector& inspector) const {
    Inspect(value, inspector);
  }
};

struct NamedNumber : Node {
  static constexpr NodeKind kKind = NodeKind::NamedNumber;

  NamedNumber() : Node(NodeKind::NamedNumber) {}

  Ident name;
  Node* value;

  void Accept(const NodeInspector& inspector) const {
    Inspect(&name, inspector);
    Inspect(value, inspector);
  }
};

struct ExceptionSpec : Node {
  static constexpr NodeKind kKind = NodeKind::ExceptionSpec;

  ExceptionSpec() : Node(NodeKind::ExceptionSpec) {}

  Node* identification;

  void Accept(const NodeInspector& inspector) const {
    Inspect(identification, inspector);
  }
};

struct Elements : Node {
  static constexpr NodeKind kKind = NodeKind::Elements;

  Elements() : Node(NodeKind::Elements) {}

  Node* content;

  void Accept(const NodeInspector& inspector) const {
    Inspect(content, inspector);
  }
};

struct AssignedOidComponent : Node {
  static constexpr NodeKind kKind = NodeKind::AssignedOidComponent;

  AssignedOidComponent() : Node(NodeKind::AssignedOidComponent) {}

  Ident name;
  Node* assignedValue;

  void Accept(const NodeInspector& inspector) const {
    Inspect(&name, inspector);
    Inspect(assignedValue, inspector);
  }
};

struct ValueLiteral : Node {
  static constexpr NodeKind kKind = NodeKind::ValueLiteral;

  ValueLiteral() : Node(NodeKind::ValueLiteral) {}

  Token tok;

  void Accept(const NodeInspector&) const {
  }
};

struct DefinedObjectClass : Node {
  static constexpr NodeKind kKind = NodeKind::DefinedObjectClass;

  DefinedObjectClass() : Node(NodeKind::DefinedObjectClass) {}

  std::optional<Ident> module;
  Ident name;

  void Accept(const NodeInspector& inspector) const {
    if (module.has_value()) {
      Inspect(std::addressof(*module), inspector);
    }
    Inspect(&name, inspector);
  }
};

struct FieldSpec : Node {
  static constexpr NodeKind kKind = NodeKind::FieldSpec;

  FieldSpec() : Node(NodeKind::FieldSpec) {}

  Ident name;
  Node* specification;

  void Accept(const NodeInspector& inspector) const {
    Inspect(&name, inspector);
    Inspect(specification, inspector);
  }
};

struct Object : Node {
  static constexpr NodeKind kKind = NodeKind::Object;

  Object() : Node(NodeKind::Object) {}

  Node* definition;

  void Accept(const NodeInspector& inspector) const {
    Inspect(definition, inspector);
  }
};

struct DefinedObject : Node {
  static constexpr NodeKind kKind = NodeKind::DefinedObject;

  DefinedObject() : Node(NodeKind::DefinedObject) {}

  Ident name;

  void Accept(const NodeInspector& inspector) const {
    Inspect(&name, inspector);
  }
};

struct SyntaxToken : Node {
  static constexpr NodeKind kKind = NodeKind::SyntaxToken;

  SyntaxToken() : Node(NodeKind::SyntaxToken) {}

  Node* content;

  void Accept(const NodeInspector& inspector) const {
    Inspect(content, inspector);
  }
};

struct ObjectIdentifierValue : Node {
  static constexpr NodeKind kKind = NodeKind::ObjectIdentifierValue;

  ObjectIdentifierValue() : Node(NodeKind::ObjectIdentifierValue) {}

  std::vector<OidComponent*> components;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : components) {
      Inspect(n, inspector);
    }
  }
};

struct ModuleExports : Node {
  static constexpr NodeKind kKind = NodeKind::ModuleExports;

  ModuleExports() : Node(NodeKind::ModuleExports) {}

  bool all{false};
  std::vector<Symbol*> symbols;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : symbols) {
      if (n != nullptr) {
        Inspect(n, inspector);
      }
    }
  }
};

struct SymbolsFromModule : Node {
  static constexpr NodeKind kKind = NodeKind::SymbolsFromModule;

  SymbolsFromModule() : Node(NodeKind::SymbolsFromModule) {}

  std::vector<Symbol*> symbols;
  Ident fromModule;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : symbols) {
      Inspect(n, inspector);
    }
    Inspect(&fromModule, inspector);
  }
};

struct ComponentType : Node {
  static constexpr NodeKind kKind = NodeKind::ComponentType;

  ComponentType() : Node(NodeKind::ComponentType) {}

  Node* content;
  bool optional{false};
  Value* defaultValue{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(content, inspector);
    if (defaultValue != nullptr) {
      Inspect(defaultValue, inspector);
    }
  }
};

struct ParameterizedAssignment : Node {
  static constexpr NodeKind kKind = NodeKind::ParameterizedAssignment;

  ParameterizedAssignment() : Node(NodeKind::ParameterizedAssignment) {}

  std::vector<Parameter*> parameters;
  Node* rhs;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : parameters) {
      Inspect(n, inspector);
    }
    Inspect(rhs, inspector);
  }
};

struct ObjectClassAssignment : Node {
  static constexpr NodeKind kKind = NodeKind::ObjectClassAssignment;

  ObjectClassAssignment() : Node(NodeKind::ObjectClassAssignment) {}

  ObjectClass* objectClass;

  void Accept(const NodeInspector& inspector) const {
    Inspect(objectClass, inspector);
  }
};

struct DefinedType : Node {
  static constexpr NodeKind kKind = NodeKind::DefinedType;

  DefinedType() : Node(NodeKind::DefinedType) {}

  Ident name;
  Ident* module{nullptr};
  std::vector<ActualParameter*> parameters;

  void Accept(const NodeInspector& inspector) const {
    Inspect(&name, inspector);
    if (module != nullptr) {
      Inspect(module, inspector);
    }
    for (const auto* n : parameters) {
      if (n != nullptr) {
        Inspect(n, inspector);
      }
    }
  }
};

struct DefinedValue : Node {
  static constexpr NodeKind kKind = NodeKind::DefinedValue;

  DefinedValue() : Node(NodeKind::DefinedValue) {}

  Ident name;
  std::optional<Ident> module;
  std::vector<ActualParameter*> parameters;

  void Accept(const NodeInspector& inspector) const {
    Inspect(&name, inspector);
    if (module.has_value()) {
      Inspect(std::addressof(*module), inspector);
    }
    for (const auto* n : parameters) {
      if (n != nullptr) {
        Inspect(n, inspector);
      }
    }
  }
};

struct EnumeratedType : Node {
  static constexpr NodeKind kKind = NodeKind::EnumeratedType;

  EnumeratedType() : Node(NodeKind::EnumeratedType) {}

  std::vector<EnumerationItem*> items;
  bool extensible{false};

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : items) {
      Inspect(n, inspector);
    }
  }
};

struct Constraint : Node {
  static constexpr NodeKind kKind = NodeKind::Constraint;

  Constraint() : Node(NodeKind::Constraint) {}

  Node* spec;
  ExceptionSpec* exception{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(spec, inspector);
    if (exception != nullptr) {
      Inspect(exception, inspector);
    }
  }
};

struct Intersection : Node {
  static constexpr NodeKind kKind = NodeKind::Intersection;

  Intersection() : Node(NodeKind::Intersection) {}

  std::vector<Elements*> elements;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : elements) {
      Inspect(n, inspector);
    }
  }
};

struct ParameterizedObject : Node {
  static constexpr NodeKind kKind = NodeKind::ParameterizedObject;

  ParameterizedObject() : Node(NodeKind::ParameterizedObject) {}

  DefinedObject* object;
  std::vector<ActualParameter*> parameters;

  void Accept(const NodeInspector& inspector) const {
    Inspect(object, inspector);
    for (const auto* n : parameters) {
      Inspect(n, inspector);
    }
  }
};

struct WithSyntaxSpec : Node {
  static constexpr NodeKind kKind = NodeKind::WithSyntaxSpec;

  WithSyntaxSpec() : Node(NodeKind::WithSyntaxSpec) {}

  std::vector<SyntaxToken*> tokens;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : tokens) {
      Inspect(n, inspector);
    }
  }
};

struct OptionalGroup : Node {
  static constexpr NodeKind kKind = NodeKind::OptionalGroup;

  OptionalGroup() : Node(NodeKind::OptionalGroup) {}

  std::vector<SyntaxToken*> tokens;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : tokens) {
      Inspect(n, inspector);
    }
  }
};

struct ModuleImports : Node {
  static constexpr NodeKind kKind = NodeKind::ModuleImports;

  ModuleImports() : Node(NodeKind::ModuleImports) {}

  std::vector<SymbolsFromModule*> symbolsFromModules;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : symbolsFromModules) {
      Inspect(n, inspector);
    }
  }
};

struct SequenceType : Node {
  static constexpr NodeKind kKind = NodeKind::SequenceType;

  SequenceType() : Node(NodeKind::SequenceType) {}

  std::vector<ComponentType*> components;
  bool extensible{false};

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : components) {
      if (n != nullptr) {
        Inspect(n, inspector);
      }
    }
  }
};

struct SetType : Node {
  static constexpr NodeKind kKind = NodeKind::SetType;

  SetType() : Node(NodeKind::SetType) {}

  std::vector<ComponentType*> components;
  bool extensible{false};

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : components) {
      if (n != nullptr) {
        Inspect(n, inspector);
      }
    }
  }
};

struct AsnType : Node {
  static constexpr NodeKind kKind = NodeKind::AsnType;

  AsnType() : Node(NodeKind::AsnType) {}

  Tag* tag{nullptr};
  Node* base;
  std::vector<Constraint*> constraints;

  void Accept(const NodeInspector& inspector) const {
    if (tag != nullptr) {
      Inspect(tag, inspector);
    }
    Inspect(base, inspector);
    for (const auto* n : constraints) {
      Inspect(n, inspector);
    }
  }
};

struct SequenceOfType : Node {
  static constexpr NodeKind kKind = NodeKind::SequenceOfType;

  SequenceOfType() : Node(NodeKind::SequenceOfType) {}

  Node* elementType;
  Constraint* sizeConstraint{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(elementType, inspector);
    if (sizeConstraint != nullptr) {
      Inspect(sizeConstraint, inspector);
    }
  }
};

struct SetOfType : Node {
  static constexpr NodeKind kKind = NodeKind::SetOfType;

  SetOfType() : Node(NodeKind::SetOfType) {}

  Node* elementType;
  Constraint* sizeConstraint{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(elementType, inspector);
    if (sizeConstraint != nullptr) {
      Inspect(sizeConstraint, inspector);
    }
  }
};

struct ElementSet : Node {
  static constexpr NodeKind kKind = NodeKind::ElementSet;

  ElementSet() : Node(NodeKind::ElementSet) {}

  std::vector<Intersection*> unions;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : unions) {
      Inspect(n, inspector);
    }
  }
};

struct ObjectClassDefn : Node {
  static constexpr NodeKind kKind = NodeKind::ObjectClassDefn;

  ObjectClassDefn() : Node(NodeKind::ObjectClassDefn) {}

  std::vector<FieldSpec*> fields;
  WithSyntaxSpec* withSyntax{nullptr};

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : fields) {
      Inspect(n, inspector);
    }
    if (withSyntax != nullptr) {
      Inspect(withSyntax, inspector);
    }
  }
};

struct Module : Node {
  static constexpr NodeKind kKind = NodeKind::Module;

  Module() : Node(NodeKind::Module) {}

  Ident name;
  std::vector<OidComponent*> oidComponents;
  Token tagDefault;
  Token extensibility;
  ModuleExports* exports{nullptr};
  ModuleImports* imports{nullptr};
  std::vector<Assignment*> assignments;

  void Accept(const NodeInspector& inspector) const {
    Inspect(&name, inspector);
    for (const auto* n : oidComponents) {
      if (n != nullptr) {
        Inspect(n, inspector);
      }
    }
    if (exports != nullptr) {
      Inspect(exports, inspector);
    }
    if (imports != nullptr) {
      Inspect(imports, inspector);
    }
    for (const auto* n : assignments) {
      Inspect(n, inspector);
    }
  }
};

struct TypeAssignment : Node {
  static constexpr NodeKind kKind = NodeKind::TypeAssignment;

  TypeAssignment() : Node(NodeKind::TypeAssignment) {}

  AsnType* type;

  void Accept(const NodeInspector& inspector) const {
    Inspect(type, inspector);
  }
};

struct ValueAssignment : Node {
  static constexpr NodeKind kKind = NodeKind::ValueAssignment;

  ValueAssignment() : Node(NodeKind::ValueAssignment) {}

  AsnType* type;
  Value* value;

  void Accept(const NodeInspector& inspector) const {
    Inspect(type, inspector);
    Inspect(value, inspector);
  }
};

struct NamedType : Node {
  static constexpr NodeKind kKind = NodeKind::NamedType;

  NamedType() : Node(NodeKind::NamedType) {}

  Ident name;
  AsnType* type;

  void Accept(const NodeInspector& inspector) const {
    Inspect(&name, inspector);
    Inspect(type, inspector);
  }
};

struct ExtensionSpec : Node {
  static constexpr NodeKind kKind = NodeKind::ExtensionSpec;

  ExtensionSpec() : Node(NodeKind::ExtensionSpec) {}

  std::vector<ElementSet*> additionalElements;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : additionalElements) {
      if (n != nullptr) {
        Inspect(n, inspector);
      }
    }
  }
};

struct ChoiceType : Node {
  static constexpr NodeKind kKind = NodeKind::ChoiceType;

  ChoiceType() : Node(NodeKind::ChoiceType) {}

  std::vector<NamedType*> alternatives;
  std::vector<NamedType*> extensionAlternatives;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : alternatives) {
      Inspect(n, inspector);
    }
    for (const auto* n : extensionAlternatives) {
      if (n != nullptr) {
        Inspect(n, inspector);
      }
    }
  }
};

struct ElementSetSpec : Node {
  static constexpr NodeKind kKind = NodeKind::ElementSetSpec;

  ElementSetSpec() : Node(NodeKind::ElementSetSpec) {}

  ElementSet* root;
  ExtensionSpec* extension{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(root, inspector);
    if (extension != nullptr) {
      Inspect(extension, inspector);
    }
  }
};

struct ObjectSetSpec : Node {
  static constexpr NodeKind kKind = NodeKind::ObjectSetSpec;

  ObjectSetSpec() : Node(NodeKind::ObjectSetSpec) {}

  ElementSet* root{nullptr};
  ExtensionSpec* extension{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (root != nullptr) {
      Inspect(root, inspector);
    }
    if (extension != nullptr) {
      Inspect(extension, inspector);
    }
  }
};

struct ObjectSet : Node {
  static constexpr NodeKind kKind = NodeKind::ObjectSet;

  ObjectSet() : Node(NodeKind::ObjectSet) {}

  ObjectSetSpec* spec;

  void Accept(const NodeInspector& inspector) const {
    Inspect(spec, inspector);
  }
};

