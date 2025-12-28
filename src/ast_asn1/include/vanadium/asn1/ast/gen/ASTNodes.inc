struct DefinitiveComponent : Node {
  static constexpr NodeKind kKind = NodeKind::DefinitiveComponent;

  DefinitiveComponent() : Node(NodeKind::DefinitiveComponent) {}

  Ident* name{nullptr};
  Token number;

  void Accept(const NodeInspector& inspector) const {
    if (name != nullptr) {
      Inspect(name, inspector);
    }
  }
};

struct SymbolReference : Node {
  static constexpr NodeKind kKind = NodeKind::SymbolReference;

  SymbolReference() : Node(NodeKind::SymbolReference) {}

  Ident ident;
  bool parametrized{false};

  void Accept(const NodeInspector& inspector) const {
    Inspect(&ident, inspector);
  }
};

struct DefinitiveComponents : Node {
  static constexpr NodeKind kKind = NodeKind::DefinitiveComponents;

  DefinitiveComponents() : Node(NodeKind::DefinitiveComponents) {}

  std::vector<DefinitiveComponent*> list;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
  }
};

struct ModuleExports : Node {
  static constexpr NodeKind kKind = NodeKind::ModuleExports;

  ModuleExports() : Node(NodeKind::ModuleExports) {}

  bool all{false};
  std::vector<SymbolReference*> list;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
  }
};

struct SymbolsFromModule : Node {
  static constexpr NodeKind kKind = NodeKind::SymbolsFromModule;

  SymbolsFromModule() : Node(NodeKind::SymbolsFromModule) {}

  Ident provider;
  std::vector<SymbolReference*> list;

  void Accept(const NodeInspector& inspector) const {
    Inspect(&provider, inspector);
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
  }
};

struct ModuleImports : Node {
  static constexpr NodeKind kKind = NodeKind::ModuleImports;

  ModuleImports() : Node(NodeKind::ModuleImports) {}

  std::vector<SymbolsFromModule*> list;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
  }
};

struct Module : Node {
  static constexpr NodeKind kKind = NodeKind::Module;

  Module() : Node(NodeKind::Module) {}

  Ident name;
  DefinitiveComponents* components{nullptr};
  Token tags;
  Token extensibility;
  ModuleExports* exports{nullptr};
  ModuleImports* imports{nullptr};
  std::vector<Node*> defs;

  void Accept(const NodeInspector& inspector) const {
    Inspect(&name, inspector);
    if (components != nullptr) {
      Inspect(components, inspector);
    }
    if (exports != nullptr) {
      Inspect(exports, inspector);
    }
    if (imports != nullptr) {
      Inspect(imports, inspector);
    }
    for (const auto* n : defs) {
      Inspect(n, inspector);
    }
  }
};

