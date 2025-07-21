struct LanguageSpec : TypeSpec {
  static constexpr NodeKind kKind = NodeKind::LanguageSpec;

  LanguageSpec() : TypeSpec(NodeKind::LanguageSpec) {}

  std::vector<Token> list;

  void Accept(const NodeInspector&) const {
  }
};

struct Definition : Node {
  static constexpr NodeKind kKind = NodeKind::Definition;

  Definition() : Node(NodeKind::Definition) {}

  Token* visibility{nullptr};
  Node* def{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (def != nullptr) {
      Inspect(def, inspector);
    }
  }
};

struct ParenExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::ParenExpr;

  ParenExpr() : Expr(NodeKind::ParenExpr) {}

  std::vector<Expr*> list;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
  }
};

struct RefSpec : TypeSpec {
  static constexpr NodeKind kKind = NodeKind::RefSpec;

  RefSpec() : TypeSpec(NodeKind::RefSpec) {}

  Expr* x;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
  }
};

struct MapSpec : TypeSpec {
  static constexpr NodeKind kKind = NodeKind::MapSpec;

  MapSpec() : TypeSpec(NodeKind::MapSpec) {}

  TypeSpec* from;
  TypeSpec* to;

  void Accept(const NodeInspector& inspector) const {
    Inspect(from, inspector);
    Inspect(to, inspector);
  }
};

struct EnumSpec : TypeSpec {
  static constexpr NodeKind kKind = NodeKind::EnumSpec;

  EnumSpec() : TypeSpec(NodeKind::EnumSpec) {}

  std::vector<Expr*> enums;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : enums) {
      Inspect(n, inspector);
    }
  }
};

struct RunsOnSpec : TypeSpec {
  static constexpr NodeKind kKind = NodeKind::RunsOnSpec;

  RunsOnSpec() : TypeSpec(NodeKind::RunsOnSpec) {}

  Ident* comp;

  void Accept(const NodeInspector& inspector) const {
    Inspect(comp, inspector);
  }
};

struct SystemSpec : TypeSpec {
  static constexpr NodeKind kKind = NodeKind::SystemSpec;

  SystemSpec() : TypeSpec(NodeKind::SystemSpec) {}

  Ident* comp;

  void Accept(const NodeInspector& inspector) const {
    Inspect(comp, inspector);
  }
};

struct RestrictionSpec : TypeSpec {
  static constexpr NodeKind kKind = NodeKind::RestrictionSpec;

  RestrictionSpec() : TypeSpec(NodeKind::RestrictionSpec) {}

  bool is_template{false};
  Token type;

  void Accept(const NodeInspector&) const {
  }
};

struct MtcSpec : TypeSpec {
  static constexpr NodeKind kKind = NodeKind::MtcSpec;

  MtcSpec() : TypeSpec(NodeKind::MtcSpec) {}

  Ident* comp;

  void Accept(const NodeInspector& inspector) const {
    Inspect(comp, inspector);
  }
};

struct BlockStmt : Stmt {
  static constexpr NodeKind kKind = NodeKind::BlockStmt;

  BlockStmt() : Stmt(NodeKind::BlockStmt) {}

  std::vector<Stmt*> stmts;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : stmts) {
      Inspect(n, inspector);
    }
  }
};

struct BranchStmt : Stmt {
  static constexpr NodeKind kKind = NodeKind::BranchStmt;

  BranchStmt() : Stmt(NodeKind::BranchStmt) {}

  Token kind;
  Ident* label{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (label != nullptr) {
      Inspect(label, inspector);
    }
  }
};

struct ReturnStmt : Stmt {
  static constexpr NodeKind kKind = NodeKind::ReturnStmt;

  ReturnStmt() : Stmt(NodeKind::ReturnStmt) {}

  Expr* result{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (result != nullptr) {
      Inspect(result, inspector);
    }
  }
};

struct WithStmt : Stmt {
  static constexpr NodeKind kKind = NodeKind::WithStmt;

  WithStmt() : Stmt(NodeKind::WithStmt) {}

  Token kind;
  bool overrides{false};
  std::vector<Expr*> list;
  Expr* value;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
    Inspect(value, inspector);
  }
};

struct ValueLiteral : Expr {
  static constexpr NodeKind kKind = NodeKind::ValueLiteral;

  ValueLiteral() : Expr(NodeKind::ValueLiteral) {}

  Token tok;

  void Accept(const NodeInspector&) const {
  }
};

struct SelectorExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::SelectorExpr;

  SelectorExpr() : Expr(NodeKind::SelectorExpr) {}

  Expr* x;
  Ident* sel{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
    if (sel != nullptr) {
      Inspect(sel, inspector);
    }
  }
};

struct DefKindExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::DefKindExpr;

  DefKindExpr() : Expr(NodeKind::DefKindExpr) {}

  Token kind;
  std::vector<Expr*> list;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
  }
};

struct ExceptExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::ExceptExpr;

  ExceptExpr() : Expr(NodeKind::ExceptExpr) {}

  Expr* x;
  std::vector<Expr*> list;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
  }
};

struct FromExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::FromExpr;

  FromExpr() : Expr(NodeKind::FromExpr) {}

  Token kind;
  Token from;
  Expr* x;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
  }
};

struct ModifiesExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::ModifiesExpr;

  ModifiesExpr() : Expr(NodeKind::ModifiesExpr) {}

  Expr* x;
  Expr* y;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
    Inspect(y, inspector);
  }
};

struct PostExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::PostExpr;

  PostExpr() : Expr(NodeKind::PostExpr) {}

  Expr* x;
  Token op;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
  }
};

struct BinaryExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::BinaryExpr;

  BinaryExpr() : Expr(NodeKind::BinaryExpr) {}

  Expr* x;
  Token op;
  Expr* y;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
    Inspect(y, inspector);
  }
};

struct UnaryExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::UnaryExpr;

  UnaryExpr() : Expr(NodeKind::UnaryExpr) {}

  Token op;
  Expr* x;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
  }
};

struct ValueExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::ValueExpr;

  ValueExpr() : Expr(NodeKind::ValueExpr) {}

  Expr* x;
  Expr* y;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
    Inspect(y, inspector);
  }
};

struct ParamExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::ParamExpr;

  ParamExpr() : Expr(NodeKind::ParamExpr) {}

  Expr* x;
  Expr* y;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
    Inspect(y, inspector);
  }
};

struct PortAttribute : Node {
  static constexpr NodeKind kKind = NodeKind::PortAttribute;

  PortAttribute() : Node(NodeKind::PortAttribute) {}

  Token kind;
  std::vector<Expr*> types;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : types) {
      Inspect(n, inspector);
    }
  }
};

struct IndexExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::IndexExpr;

  IndexExpr() : Expr(NodeKind::IndexExpr) {}

  Expr* x{nullptr};
  Expr* index;

  void Accept(const NodeInspector& inspector) const {
    if (x != nullptr) {
      Inspect(x, inspector);
    }
    Inspect(index, inspector);
  }
};

struct RedirectToIndex : Node {
  static constexpr NodeKind kKind = NodeKind::RedirectToIndex;

  RedirectToIndex() : Node(NodeKind::RedirectToIndex) {}

  bool value{false};
  Expr* index;

  void Accept(const NodeInspector& inspector) const {
    Inspect(index, inspector);
  }
};

struct CompositeLiteral : Expr {
  static constexpr NodeKind kKind = NodeKind::CompositeLiteral;

  CompositeLiteral() : Expr(NodeKind::CompositeLiteral) {}

  std::vector<Expr*> list;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
  }
};

struct RegexpExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::RegexpExpr;

  RegexpExpr() : Expr(NodeKind::RegexpExpr) {}

  bool nocase{false};
  Expr* x;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
  }
};

struct PatternExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::PatternExpr;

  PatternExpr() : Expr(NodeKind::PatternExpr) {}

  bool nocase{false};
  Expr* x;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
  }
};

struct DecodedExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::DecodedExpr;

  DecodedExpr() : Expr(NodeKind::DecodedExpr) {}

  Expr* params{nullptr};
  Expr* x;

  void Accept(const NodeInspector& inspector) const {
    if (params != nullptr) [[likely]] {
      Inspect(params, inspector);
    }
    Inspect(x, inspector);
  }
};

struct DecmatchExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::DecmatchExpr;

  DecmatchExpr() : Expr(NodeKind::DecmatchExpr) {}

  Expr* params{nullptr};
  Expr* x;

  void Accept(const NodeInspector& inspector) const {
    if (params != nullptr) [[likely]] {
      Inspect(params, inspector);
    }
    Inspect(x, inspector);
  }
};

struct AssignmentExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::AssignmentExpr;

  AssignmentExpr() : Expr(NodeKind::AssignmentExpr) {}

  Expr* property;
  Expr* value;

  void Accept(const NodeInspector& inspector) const {
    Inspect(property, inspector);
    Inspect(value, inspector);
  }
};

struct Declarator : Node {
  static constexpr NodeKind kKind = NodeKind::Declarator;

  Declarator() : Node(NodeKind::Declarator) {}

  std::optional<Ident> name;
  std::vector<ParenExpr*> arraydef;
  Expr* value{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (name.has_value()) {
      Inspect(std::addressof(*name), inspector);
    }
    for (const auto* n : arraydef) {
      if (n != nullptr) {
        Inspect(n, inspector);
      }
    }
    if (value != nullptr) {
      Inspect(value, inspector);
    }
  }
};

struct LengthExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::LengthExpr;

  LengthExpr() : Expr(NodeKind::LengthExpr) {}

  Expr* x{nullptr};
  ParenExpr* size;

  void Accept(const NodeInspector& inspector) const {
    if (x != nullptr) {
      Inspect(x, inspector);
    }
    Inspect(size, inspector);
  }
};

struct CallExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::CallExpr;

  CallExpr() : Expr(NodeKind::CallExpr) {}

  Expr* fun;
  ParenExpr* args;

  void Accept(const NodeInspector& inspector) const {
    Inspect(fun, inspector);
    Inspect(args, inspector);
  }
};

struct ParametrizedIdent : Ident {
  static constexpr NodeKind kKind = NodeKind::ParametrizedIdent;

  ParametrizedIdent() : Ident(NodeKind::ParametrizedIdent) {}

  Ident* ident;
  ParenExpr* params;

  void Accept(const NodeInspector& inspector) const {
    Inspect(ident, inspector);
    Inspect(params, inspector);
  }
};

struct FormalPar : Node {
  static constexpr NodeKind kKind = NodeKind::FormalPar;

  FormalPar() : Node(NodeKind::FormalPar) {}

  Token* direction{nullptr};
  RestrictionSpec* restriction{nullptr};
  Token* modif{nullptr};
  Expr* type;
  std::optional<Ident> name;
  std::vector<ParenExpr*> arraydef;
  Expr* value{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (restriction != nullptr) {
      Inspect(restriction, inspector);
    }
    Inspect(type, inspector);
    if (name.has_value()) {
      Inspect(std::addressof(*name), inspector);
    }
    for (const auto* n : arraydef) {
      if (n != nullptr) {
        Inspect(n, inspector);
      }
    }
    if (value != nullptr) {
      Inspect(value, inspector);
    }
  }
};

struct ReturnSpec : TypeSpec {
  static constexpr NodeKind kKind = NodeKind::ReturnSpec;

  ReturnSpec() : TypeSpec(NodeKind::ReturnSpec) {}

  RestrictionSpec* restriction{nullptr};
  Token* modif{nullptr};
  Expr* type;

  void Accept(const NodeInspector& inspector) const {
    if (restriction != nullptr) {
      Inspect(restriction, inspector);
    }
    Inspect(type, inspector);
  }
};

struct AltStmt : Stmt {
  static constexpr NodeKind kKind = NodeKind::AltStmt;

  AltStmt() : Stmt(NodeKind::AltStmt) {}

  Token kind;
  Token* no_default{nullptr};
  BlockStmt* body;

  void Accept(const NodeInspector& inspector) const {
    Inspect(body, inspector);
  }
};

struct CallStmt : Stmt {
  static constexpr NodeKind kKind = NodeKind::CallStmt;

  CallStmt() : Stmt(NodeKind::CallStmt) {}

  Stmt* stmt;
  BlockStmt* body;

  void Accept(const NodeInspector& inspector) const {
    Inspect(stmt, inspector);
    Inspect(body, inspector);
  }
};

struct ForStmt : Stmt {
  static constexpr NodeKind kKind = NodeKind::ForStmt;

  ForStmt() : Stmt(NodeKind::ForStmt) {}

  Stmt* init;
  Expr* cond;
  Stmt* post;
  BlockStmt* body;

  void Accept(const NodeInspector& inspector) const {
    Inspect(init, inspector);
    Inspect(cond, inspector);
    Inspect(post, inspector);
    Inspect(body, inspector);
  }
};

struct ForRangeStmt : Stmt {
  static constexpr NodeKind kKind = NodeKind::ForRangeStmt;

  ForRangeStmt() : Stmt(NodeKind::ForRangeStmt) {}

  Stmt* init;
  Expr* range;
  BlockStmt* body;

  void Accept(const NodeInspector& inspector) const {
    Inspect(init, inspector);
    Inspect(range, inspector);
    Inspect(body, inspector);
  }
};

struct WhileStmt : Stmt {
  static constexpr NodeKind kKind = NodeKind::WhileStmt;

  WhileStmt() : Stmt(NodeKind::WhileStmt) {}

  Expr* cond{nullptr};
  BlockStmt* body{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (cond != nullptr) {
      Inspect(cond, inspector);
    }
    if (body != nullptr) {
      Inspect(body, inspector);
    }
  }
};

struct DoWhileStmt : Stmt {
  static constexpr NodeKind kKind = NodeKind::DoWhileStmt;

  DoWhileStmt() : Stmt(NodeKind::DoWhileStmt) {}

  BlockStmt* body{nullptr};
  Expr* cond{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (body != nullptr) {
      Inspect(body, inspector);
    }
    if (cond != nullptr) {
      Inspect(cond, inspector);
    }
  }
};

struct IfStmt : Stmt {
  static constexpr NodeKind kKind = NodeKind::IfStmt;

  IfStmt() : Stmt(NodeKind::IfStmt) {}

  Expr* cond;
  BlockStmt* consequent;
  Stmt* alternate{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(cond, inspector);
    Inspect(consequent, inspector);
    if (alternate != nullptr) {
      Inspect(alternate, inspector);
    }
  }
};

struct CaseClause : Node {
  static constexpr NodeKind kKind = NodeKind::CaseClause;

  CaseClause() : Node(NodeKind::CaseClause) {}

  ParenExpr* cond{nullptr};
  BlockStmt* body;

  void Accept(const NodeInspector& inspector) const {
    if (cond != nullptr) {
      Inspect(cond, inspector);
    }
    Inspect(body, inspector);
  }
};

struct CommClause : Stmt {
  static constexpr NodeKind kKind = NodeKind::CommClause;

  CommClause() : Stmt(NodeKind::CommClause) {}

  bool is_else{false};
  Expr* x{nullptr};
  Stmt* comm;
  BlockStmt* body{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (x != nullptr) {
      Inspect(x, inspector);
    }
    Inspect(comm, inspector);
    if (body != nullptr) {
      Inspect(body, inspector);
    }
  }
};

struct DynamicExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::DynamicExpr;

  DynamicExpr() : Expr(NodeKind::DynamicExpr) {}

  BlockStmt* body;

  void Accept(const NodeInspector& inspector) const {
    Inspect(body, inspector);
  }
};

struct WithSpec : TypeSpec {
  static constexpr NodeKind kKind = NodeKind::WithSpec;

  WithSpec() : TypeSpec(NodeKind::WithSpec) {}

  std::vector<WithStmt*> list;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
  }
};

struct RedirectExpr : Expr {
  static constexpr NodeKind kKind = NodeKind::RedirectExpr;

  RedirectExpr() : Expr(NodeKind::RedirectExpr) {}

  Expr* x;
  std::vector<Expr*> value;
  std::vector<Expr*> param;
  Expr* sender{nullptr};
  RedirectToIndex* to_index{nullptr};
  Expr* timestamp{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
    for (const auto* n : value) {
      if (n != nullptr) {
        Inspect(n, inspector);
      }
    }
    for (const auto* n : param) {
      if (n != nullptr) {
        Inspect(n, inspector);
      }
    }
    if (sender != nullptr) {
      Inspect(sender, inspector);
    }
    if (to_index != nullptr) {
      Inspect(to_index, inspector);
    }
    if (timestamp != nullptr) {
      Inspect(timestamp, inspector);
    }
  }
};

struct ListSpec : TypeSpec {
  static constexpr NodeKind kKind = NodeKind::ListSpec;

  ListSpec() : TypeSpec(NodeKind::ListSpec) {}

  LengthExpr* length{nullptr};
  TypeSpec* elemtype;

  void Accept(const NodeInspector& inspector) const {
    if (length != nullptr) {
      Inspect(length, inspector);
    }
    Inspect(elemtype, inspector);
  }
};

struct FormalPars : Node {
  static constexpr NodeKind kKind = NodeKind::FormalPars;

  FormalPars() : Node(NodeKind::FormalPars) {}

  std::vector<FormalPar*> list;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
  }
};

struct SelectStmt : Stmt {
  static constexpr NodeKind kKind = NodeKind::SelectStmt;

  SelectStmt() : Stmt(NodeKind::SelectStmt) {}

  bool is_union{false};
  ParenExpr* tag;
  std::vector<CaseClause*> clauses;

  void Accept(const NodeInspector& inspector) const {
    Inspect(tag, inspector);
    for (const auto* n : clauses) {
      Inspect(n, inspector);
    }
  }
};

struct Module : Node {
  static constexpr NodeKind kKind = NodeKind::Module;

  Module() : Node(NodeKind::Module) {}

  std::optional<Ident> name;
  LanguageSpec* language{nullptr};
  std::vector<Definition*> defs;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (name.has_value()) {
      Inspect(std::addressof(*name), inspector);
    }
    if (language != nullptr) {
      Inspect(language, inspector);
    }
    for (const auto* n : defs) {
      Inspect(n, inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct ValueDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::ValueDecl;

  ValueDecl() : Decl(NodeKind::ValueDecl) {}

  Token* kind{nullptr};
  RestrictionSpec* template_restriction{nullptr};
  Token* modif{nullptr};
  Expr* type;
  std::vector<Declarator*> decls;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (template_restriction != nullptr) {
      Inspect(template_restriction, inspector);
    }
    Inspect(type, inspector);
    for (const auto* n : decls) {
      Inspect(n, inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct ImportDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::ImportDecl;

  ImportDecl() : Decl(NodeKind::ImportDecl) {}

  std::optional<Ident> module;
  LanguageSpec* language{nullptr};
  std::vector<DefKindExpr*> list;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (module.has_value()) {
      Inspect(std::addressof(*module), inspector);
    }
    if (language != nullptr) {
      Inspect(language, inspector);
    }
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct GroupDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::GroupDecl;

  GroupDecl() : Decl(NodeKind::GroupDecl) {}

  std::optional<Ident> name;
  std::vector<Definition*> defs;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (name.has_value()) {
      Inspect(std::addressof(*name), inspector);
    }
    for (const auto* n : defs) {
      Inspect(n, inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct FriendDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::FriendDecl;

  FriendDecl() : Decl(NodeKind::FriendDecl) {}

  std::optional<Ident> module;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (module.has_value()) {
      Inspect(std::addressof(*module), inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct ClassTypeDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::ClassTypeDecl;

  ClassTypeDecl() : Decl(NodeKind::ClassTypeDecl) {}

  Token kind;
  Token* modif{nullptr};
  std::optional<Ident> name;
  std::vector<Expr*> extends;
  RunsOnSpec* runs_on{nullptr};
  MtcSpec* mtc{nullptr};
  SystemSpec* system{nullptr};
  std::vector<Definition*> defs;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (name.has_value()) {
      Inspect(std::addressof(*name), inspector);
    }
    for (const auto* n : extends) {
      if (n != nullptr) {
        Inspect(n, inspector);
      }
    }
    if (runs_on != nullptr) {
      Inspect(runs_on, inspector);
    }
    if (mtc != nullptr) {
      Inspect(mtc, inspector);
    }
    if (system != nullptr) {
      Inspect(system, inspector);
    }
    for (const auto* n : defs) {
      Inspect(n, inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct ControlPart : Node {
  static constexpr NodeKind kKind = NodeKind::ControlPart;

  ControlPart() : Node(NodeKind::ControlPart) {}

  BlockStmt* body;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(body, inspector);
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct Field : Node {
  static constexpr NodeKind kKind = NodeKind::Field;

  Field() : Node(NodeKind::Field) {}

  Token* default_tok{nullptr};
  TypeSpec* type;
  std::optional<Ident> name;
  std::vector<ParenExpr*> arraydef;
  FormalPars* pars{nullptr};
  ParenExpr* value_constraint{nullptr};
  bool optional{false};

  void Accept(const NodeInspector& inspector) const {
    Inspect(type, inspector);
    if (name.has_value()) {
      Inspect(std::addressof(*name), inspector);
    }
    for (const auto* n : arraydef) {
      if (n != nullptr) {
        Inspect(n, inspector);
      }
    }
    if (pars != nullptr) {
      Inspect(pars, inspector);
    }
    if (value_constraint != nullptr) {
      Inspect(value_constraint, inspector);
    }
  }
};

struct BehaviourSpec : TypeSpec {
  static constexpr NodeKind kKind = NodeKind::BehaviourSpec;

  BehaviourSpec() : TypeSpec(NodeKind::BehaviourSpec) {}

  Token kind;
  FormalPars* params;
  RunsOnSpec* runs_on{nullptr};
  SystemSpec* system{nullptr};
  ReturnSpec* ret{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(params, inspector);
    if (runs_on != nullptr) {
      Inspect(runs_on, inspector);
    }
    if (system != nullptr) {
      Inspect(system, inspector);
    }
    if (ret != nullptr) {
      Inspect(ret, inspector);
    }
  }
};

struct TemplateDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::TemplateDecl;

  TemplateDecl() : Decl(NodeKind::TemplateDecl) {}

  RestrictionSpec* restriction{nullptr};
  Token* modif{nullptr};
  Expr* type;
  std::optional<Ident> name;
  FormalPars* pars{nullptr};
  FormalPars* params{nullptr};
  Expr* base{nullptr};
  Expr* value;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (restriction != nullptr) {
      Inspect(restriction, inspector);
    }
    Inspect(type, inspector);
    if (name.has_value()) {
      Inspect(std::addressof(*name), inspector);
    }
    if (pars != nullptr) {
      Inspect(pars, inspector);
    }
    if (params != nullptr) [[likely]] {
      Inspect(params, inspector);
    }
    if (base != nullptr) {
      Inspect(base, inspector);
    }
    Inspect(value, inspector);
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct FuncDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::FuncDecl;

  FuncDecl() : Decl(NodeKind::FuncDecl) {}

  bool external{false};
  Token kind;
  std::optional<Ident> name;
  Token* modif{nullptr};
  FormalPars* pars{nullptr};
  FormalPars* params{nullptr};
  RunsOnSpec* runs_on{nullptr};
  MtcSpec* mtc{nullptr};
  SystemSpec* system{nullptr};
  ReturnSpec* ret{nullptr};
  BlockStmt* body{nullptr};
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (name.has_value()) {
      Inspect(std::addressof(*name), inspector);
    }
    if (pars != nullptr) {
      Inspect(pars, inspector);
    }
    if (params != nullptr) [[likely]] {
      Inspect(params, inspector);
    }
    if (runs_on != nullptr) {
      Inspect(runs_on, inspector);
    }
    if (mtc != nullptr) {
      Inspect(mtc, inspector);
    }
    if (system != nullptr) {
      Inspect(system, inspector);
    }
    if (ret != nullptr) {
      Inspect(ret, inspector);
    }
    if (body != nullptr) {
      Inspect(body, inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct ConstructorDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::ConstructorDecl;

  ConstructorDecl() : Decl(NodeKind::ConstructorDecl) {}

  FormalPars* params{nullptr};
  BlockStmt* body;

  void Accept(const NodeInspector& inspector) const {
    if (params != nullptr) [[likely]] {
      Inspect(params, inspector);
    }
    Inspect(body, inspector);
  }
};

struct SignatureDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::SignatureDecl;

  SignatureDecl() : Decl(NodeKind::SignatureDecl) {}

  std::optional<Ident> name;
  FormalPars* pars{nullptr};
  FormalPars* params;
  bool noblock{false};
  ReturnSpec* ret{nullptr};
  ParenExpr* exception{nullptr};
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (name.has_value()) {
      Inspect(std::addressof(*name), inspector);
    }
    if (pars != nullptr) {
      Inspect(pars, inspector);
    }
    Inspect(params, inspector);
    if (ret != nullptr) {
      Inspect(ret, inspector);
    }
    if (exception != nullptr) {
      Inspect(exception, inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct MapTypeDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::MapTypeDecl;

  MapTypeDecl() : Decl(NodeKind::MapTypeDecl) {}

  MapSpec* spec;
  std::optional<Ident> name;
  FormalPars* pars{nullptr};
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(spec, inspector);
    if (name.has_value()) {
      Inspect(std::addressof(*name), inspector);
    }
    if (pars != nullptr) {
      Inspect(pars, inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct EnumTypeDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::EnumTypeDecl;

  EnumTypeDecl() : Decl(NodeKind::EnumTypeDecl) {}

  std::optional<Ident> name;
  FormalPars* pars{nullptr};
  std::vector<Expr*> enums;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (name.has_value()) {
      Inspect(std::addressof(*name), inspector);
    }
    if (pars != nullptr) {
      Inspect(pars, inspector);
    }
    for (const auto* n : enums) {
      Inspect(n, inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct BehaviourTypeDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::BehaviourTypeDecl;

  BehaviourTypeDecl() : Decl(NodeKind::BehaviourTypeDecl) {}

  Token kind;
  std::optional<Ident> name;
  FormalPars* pars{nullptr};
  FormalPars* params;
  RunsOnSpec* runs_on{nullptr};
  SystemSpec* system{nullptr};
  ReturnSpec* ret{nullptr};
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (name.has_value()) {
      Inspect(std::addressof(*name), inspector);
    }
    if (pars != nullptr) {
      Inspect(pars, inspector);
    }
    Inspect(params, inspector);
    if (runs_on != nullptr) {
      Inspect(runs_on, inspector);
    }
    if (system != nullptr) {
      Inspect(system, inspector);
    }
    if (ret != nullptr) {
      Inspect(ret, inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct PortTypeDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::PortTypeDecl;

  PortTypeDecl() : Decl(NodeKind::PortTypeDecl) {}

  std::optional<Ident> name;
  FormalPars* pars{nullptr};
  bool realtime{false};
  std::vector<Node*> attrs;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (name.has_value()) {
      Inspect(std::addressof(*name), inspector);
    }
    if (pars != nullptr) {
      Inspect(pars, inspector);
    }
    for (const auto* n : attrs) {
      Inspect(n, inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct PortMapAttribute : Node {
  static constexpr NodeKind kKind = NodeKind::PortMapAttribute;

  PortMapAttribute() : Node(NodeKind::PortMapAttribute) {}

  Token kind;
  FormalPars* params;

  void Accept(const NodeInspector& inspector) const {
    Inspect(params, inspector);
  }
};

struct ComponentTypeDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::ComponentTypeDecl;

  ComponentTypeDecl() : Decl(NodeKind::ComponentTypeDecl) {}

  std::optional<Ident> name;
  FormalPars* pars{nullptr};
  std::vector<Expr*> extends;
  BlockStmt* body;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (name.has_value()) {
      Inspect(std::addressof(*name), inspector);
    }
    if (pars != nullptr) {
      Inspect(pars, inspector);
    }
    for (const auto* n : extends) {
      if (n != nullptr) {
        Inspect(n, inspector);
      }
    }
    Inspect(body, inspector);
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct ModuleParameterGroup : Decl {
  static constexpr NodeKind kKind = NodeKind::ModuleParameterGroup;

  ModuleParameterGroup() : Decl(NodeKind::ModuleParameterGroup) {}

  std::vector<ValueDecl*> decls;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : decls) {
      Inspect(n, inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct StructSpec : TypeSpec {
  static constexpr NodeKind kKind = NodeKind::StructSpec;

  StructSpec() : TypeSpec(NodeKind::StructSpec) {}

  Token kind;
  std::vector<Field*> fields;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : fields) {
      Inspect(n, inspector);
    }
  }
};

struct SubTypeDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::SubTypeDecl;

  SubTypeDecl() : Decl(NodeKind::SubTypeDecl) {}

  Field* field;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(field, inspector);
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct StructTypeDecl : Decl {
  static constexpr NodeKind kKind = NodeKind::StructTypeDecl;

  StructTypeDecl() : Decl(NodeKind::StructTypeDecl) {}

  Token kind;
  std::optional<Ident> name;
  FormalPars* pars{nullptr};
  std::vector<Field*> fields;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (name.has_value()) {
      Inspect(std::addressof(*name), inspector);
    }
    if (pars != nullptr) {
      Inspect(pars, inspector);
    }
    for (const auto* n : fields) {
      Inspect(n, inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

