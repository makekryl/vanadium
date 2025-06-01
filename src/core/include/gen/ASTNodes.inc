struct LanguageSpec : TypeSpec {
  LanguageSpec() : TypeSpec(NodeKind::LanguageSpec) {}

  std::vector<Token> list;

  void Accept(const NodeInspector&) const {
  }
};

struct ModuleDef : Node {
  ModuleDef() : Node(NodeKind::ModuleDef) {}

  Token* visibility{nullptr};
  Node* def;

  void Accept(const NodeInspector& inspector) const {
    Inspect(def, inspector);
  }
};

struct ParenExpr : Expr {
  ParenExpr() : Expr(NodeKind::ParenExpr) {}

  std::vector<Expr*> list;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
  }
};

struct RefSpec : TypeSpec {
  RefSpec() : TypeSpec(NodeKind::RefSpec) {}

  Expr* x;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
  }
};

struct MapSpec : TypeSpec {
  MapSpec() : TypeSpec(NodeKind::MapSpec) {}

  TypeSpec* from;
  TypeSpec* to;

  void Accept(const NodeInspector& inspector) const {
    Inspect(from, inspector);
    Inspect(to, inspector);
  }
};

struct EnumSpec : TypeSpec {
  EnumSpec() : TypeSpec(NodeKind::EnumSpec) {}

  std::vector<Expr*> enums;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : enums) {
      Inspect(n, inspector);
    }
  }
};

struct RunsOnSpec : TypeSpec {
  RunsOnSpec() : TypeSpec(NodeKind::RunsOnSpec) {}

  Ident* comp;

  void Accept(const NodeInspector& inspector) const {
    Inspect(comp, inspector);
  }
};

struct SystemSpec : TypeSpec {
  SystemSpec() : TypeSpec(NodeKind::SystemSpec) {}

  Ident* comp;

  void Accept(const NodeInspector& inspector) const {
    Inspect(comp, inspector);
  }
};

struct RestrictionSpec : TypeSpec {
  RestrictionSpec() : TypeSpec(NodeKind::RestrictionSpec) {}

  std::optional<bool> is_template{false};
  Token type;

  void Accept(const NodeInspector&) const {
  }
};

struct MtcSpec : TypeSpec {
  MtcSpec() : TypeSpec(NodeKind::MtcSpec) {}

  Ident* comp;

  void Accept(const NodeInspector& inspector) const {
    Inspect(comp, inspector);
  }
};

struct BlockStmt : Stmt {
  BlockStmt() : Stmt(NodeKind::BlockStmt) {}

  std::vector<Stmt*> stmts;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : stmts) {
      Inspect(n, inspector);
    }
  }
};

struct BranchStmt : Stmt {
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
  ReturnStmt() : Stmt(NodeKind::ReturnStmt) {}

  Expr* result{nullptr};

  void Accept(const NodeInspector& inspector) const {
    if (result != nullptr) {
      Inspect(result, inspector);
    }
  }
};

struct WithStmt : Stmt {
  WithStmt() : Stmt(NodeKind::WithStmt) {}

  Token kind;
  std::optional<bool> overrides{false};
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
  ValueLiteral() : Expr(NodeKind::ValueLiteral) {}

  Token tok;

  void Accept(const NodeInspector&) const {
  }
};

struct SelectorExpr : Expr {
  SelectorExpr() : Expr(NodeKind::SelectorExpr) {}

  Expr* x;
  Expr* sel;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
    Inspect(sel, inspector);
  }
};

struct DefKindExpr : Expr {
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
  FromExpr() : Expr(NodeKind::FromExpr) {}

  Token kind;
  Token from;
  Expr* x;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
  }
};

struct ModifiesExpr : Expr {
  ModifiesExpr() : Expr(NodeKind::ModifiesExpr) {}

  Expr* x;
  Expr* y;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
    Inspect(y, inspector);
  }
};

struct PostExpr : Expr {
  PostExpr() : Expr(NodeKind::PostExpr) {}

  Expr* x;
  Token op;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
  }
};

struct BinaryExpr : Expr {
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
  UnaryExpr() : Expr(NodeKind::UnaryExpr) {}

  Token op;
  Expr* x;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
  }
};

struct ValueExpr : Expr {
  ValueExpr() : Expr(NodeKind::ValueExpr) {}

  Expr* x;
  Expr* y;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
    Inspect(y, inspector);
  }
};

struct ParamExpr : Expr {
  ParamExpr() : Expr(NodeKind::ParamExpr) {}

  Expr* x;
  Expr* y;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
    Inspect(y, inspector);
  }
};

struct PortAttribute : Node {
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
  RedirectToIndex() : Node(NodeKind::RedirectToIndex) {}

  std::optional<bool> value{false};
  Expr* index;

  void Accept(const NodeInspector& inspector) const {
    Inspect(index, inspector);
  }
};

struct CompositeLiteral : Expr {
  CompositeLiteral() : Expr(NodeKind::CompositeLiteral) {}

  std::vector<Expr*> list;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
  }
};

struct RegexpExpr : Expr {
  RegexpExpr() : Expr(NodeKind::RegexpExpr) {}

  std::optional<bool> nocase{false};
  Expr* x;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
  }
};

struct PatternExpr : Expr {
  PatternExpr() : Expr(NodeKind::PatternExpr) {}

  std::optional<bool> nocase{false};
  Expr* x;

  void Accept(const NodeInspector& inspector) const {
    Inspect(x, inspector);
  }
};

struct DecodedExpr : Expr {
  DecodedExpr() : Expr(NodeKind::DecodedExpr) {}

  Expr* params{nullptr};
  Expr* x;

  void Accept(const NodeInspector& inspector) const {
    if (params != nullptr) {
      Inspect(params, inspector);
    }
    Inspect(x, inspector);
  }
};

struct DecmatchExpr : Expr {
  DecmatchExpr() : Expr(NodeKind::DecmatchExpr) {}

  Expr* params{nullptr};
  Expr* x;

  void Accept(const NodeInspector& inspector) const {
    if (params != nullptr) {
      Inspect(params, inspector);
    }
    Inspect(x, inspector);
  }
};

struct AssignmentExpr : Expr {
  AssignmentExpr() : Expr(NodeKind::AssignmentExpr) {}

  Expr* property;
  Expr* value;

  void Accept(const NodeInspector& inspector) const {
    Inspect(property, inspector);
    Inspect(value, inspector);
  }
};

struct Declarator : Node {
  Declarator() : Node(NodeKind::Declarator) {}

  std::optional<Ident> name;
  std::vector<ParenExpr*> arraydef;
  Expr* value{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(std::addressof(*name), inspector);
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
  CallExpr() : Expr(NodeKind::CallExpr) {}

  Expr* fun;
  ParenExpr* args;

  void Accept(const NodeInspector& inspector) const {
    Inspect(fun, inspector);
    Inspect(args, inspector);
  }
};

struct ParametrizedIdent : Expr {
  ParametrizedIdent() : Expr(NodeKind::ParametrizedIdent) {}

  Ident* ident;
  ParenExpr* params;

  void Accept(const NodeInspector& inspector) const {
    Inspect(ident, inspector);
    Inspect(params, inspector);
  }
};

struct FormalPar : Node {
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
    Inspect(std::addressof(*name), inspector);
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
  AltStmt() : Stmt(NodeKind::AltStmt) {}

  Token kind;
  Token* no_default{nullptr};
  BlockStmt* body;

  void Accept(const NodeInspector& inspector) const {
    Inspect(body, inspector);
  }
};

struct CallStmt : Stmt {
  CallStmt() : Stmt(NodeKind::CallStmt) {}

  Stmt* stmt;
  BlockStmt* body;

  void Accept(const NodeInspector& inspector) const {
    Inspect(stmt, inspector);
    Inspect(body, inspector);
  }
};

struct ForStmt : Stmt {
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
  WhileStmt() : Stmt(NodeKind::WhileStmt) {}

  Expr* cond;
  BlockStmt* body;

  void Accept(const NodeInspector& inspector) const {
    Inspect(cond, inspector);
    Inspect(body, inspector);
  }
};

struct DoWhileStmt : Stmt {
  DoWhileStmt() : Stmt(NodeKind::DoWhileStmt) {}

  BlockStmt* body;
  Expr* cond;

  void Accept(const NodeInspector& inspector) const {
    Inspect(body, inspector);
    Inspect(cond, inspector);
  }
};

struct IfStmt : Stmt {
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
  CommClause() : Stmt(NodeKind::CommClause) {}

  std::optional<bool> is_else{false};
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
  DynamicExpr() : Expr(NodeKind::DynamicExpr) {}

  BlockStmt* body;

  void Accept(const NodeInspector& inspector) const {
    Inspect(body, inspector);
  }
};

struct WithSpec : TypeSpec {
  WithSpec() : TypeSpec(NodeKind::WithSpec) {}

  std::vector<WithStmt*> list;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
  }
};

struct RedirectExpr : Expr {
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
  FormalPars() : Node(NodeKind::FormalPars) {}

  std::vector<FormalPar*> list;

  void Accept(const NodeInspector& inspector) const {
    for (const auto* n : list) {
      Inspect(n, inspector);
    }
  }
};

struct SelectStmt : Stmt {
  SelectStmt() : Stmt(NodeKind::SelectStmt) {}

  std::optional<bool> is_union{false};
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
  Module() : Node(NodeKind::Module) {}

  std::optional<Ident> name;
  LanguageSpec* language{nullptr};
  std::vector<ModuleDef*> defs;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(std::addressof(*name), inspector);
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
  ImportDecl() : Decl(NodeKind::ImportDecl) {}

  std::optional<Ident> module;
  LanguageSpec* language{nullptr};
  std::vector<DefKindExpr*> list;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(std::addressof(*module), inspector);
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
  GroupDecl() : Decl(NodeKind::GroupDecl) {}

  std::optional<Ident> name;
  std::vector<ModuleDef*> defs;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(std::addressof(*name), inspector);
    for (const auto* n : defs) {
      Inspect(n, inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct FriendDecl : Decl {
  FriendDecl() : Decl(NodeKind::FriendDecl) {}

  std::optional<Ident> module;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(std::addressof(*module), inspector);
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct ClassTypeDecl : Decl {
  ClassTypeDecl() : Decl(NodeKind::ClassTypeDecl) {}

  Token* modif{nullptr};
  std::optional<Ident> name;
  std::vector<Expr*> extends;
  RunsOnSpec* runs_on{nullptr};
  MtcSpec* mtc{nullptr};
  SystemSpec* system{nullptr};
  std::vector<ModuleDef*> defs;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(std::addressof(*name), inspector);
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
  Field() : Node(NodeKind::Field) {}

  Token* default_tok{nullptr};
  TypeSpec* type;
  std::optional<Ident> name;
  std::vector<ParenExpr*> arraydef;
  FormalPars* pars{nullptr};
  ParenExpr* value_constraint{nullptr};
  LengthExpr* length_constraint{nullptr};
  std::optional<bool> optional{false};

  void Accept(const NodeInspector& inspector) const {
    Inspect(type, inspector);
    Inspect(std::addressof(*name), inspector);
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
    if (length_constraint != nullptr) {
      Inspect(length_constraint, inspector);
    }
  }
};

struct BehaviourSpec : TypeSpec {
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
    Inspect(std::addressof(*name), inspector);
    if (pars != nullptr) {
      Inspect(pars, inspector);
    }
    if (params != nullptr) {
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
  FuncDecl() : Decl(NodeKind::FuncDecl) {}

  std::optional<bool> external{false};
  Token kind;
  std::optional<Ident> name;
  Token* modif{nullptr};
  FormalPars* pars{nullptr};
  FormalPars* params;
  RunsOnSpec* runs_on{nullptr};
  MtcSpec* mtc{nullptr};
  SystemSpec* system{nullptr};
  ReturnSpec* ret{nullptr};
  BlockStmt* body{nullptr};
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(std::addressof(*name), inspector);
    if (pars != nullptr) {
      Inspect(pars, inspector);
    }
    Inspect(params, inspector);
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
  ConstructorDecl() : Decl(NodeKind::ConstructorDecl) {}

  FormalPars* params;
  BlockStmt* body;

  void Accept(const NodeInspector& inspector) const {
    Inspect(params, inspector);
    Inspect(body, inspector);
  }
};

struct SignatureDecl : Decl {
  SignatureDecl() : Decl(NodeKind::SignatureDecl) {}

  std::optional<Ident> name;
  FormalPars* pars{nullptr};
  FormalPars* params;
  std::optional<bool> noblock{false};
  ReturnSpec* ret{nullptr};
  ParenExpr* exception{nullptr};
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(std::addressof(*name), inspector);
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
  MapTypeDecl() : Decl(NodeKind::MapTypeDecl) {}

  MapSpec* spec;
  std::optional<Ident> name;
  FormalPars* pars{nullptr};
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(spec, inspector);
    Inspect(std::addressof(*name), inspector);
    if (pars != nullptr) {
      Inspect(pars, inspector);
    }
    if (with != nullptr) {
      Inspect(with, inspector);
    }
  }
};

struct EnumTypeDecl : Decl {
  EnumTypeDecl() : Decl(NodeKind::EnumTypeDecl) {}

  std::optional<Ident> name;
  FormalPars* pars{nullptr};
  std::vector<Expr*> enums;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(std::addressof(*name), inspector);
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
    Inspect(std::addressof(*name), inspector);
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
  PortTypeDecl() : Decl(NodeKind::PortTypeDecl) {}

  std::optional<Ident> name;
  FormalPars* pars{nullptr};
  std::optional<bool> realtime{false};
  std::vector<Node*> attrs;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(std::addressof(*name), inspector);
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
  PortMapAttribute() : Node(NodeKind::PortMapAttribute) {}

  Token kind;
  FormalPars* params;

  void Accept(const NodeInspector& inspector) const {
    Inspect(params, inspector);
  }
};

struct ComponentTypeDecl : Decl {
  ComponentTypeDecl() : Decl(NodeKind::ComponentTypeDecl) {}

  std::optional<Ident> name;
  FormalPars* pars{nullptr};
  std::vector<Expr*> extends;
  BlockStmt* body;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(std::addressof(*name), inspector);
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
  StructTypeDecl() : Decl(NodeKind::StructTypeDecl) {}

  Token kind;
  std::optional<Ident> name;
  FormalPars* pars{nullptr};
  std::vector<Field*> fields;
  WithSpec* with{nullptr};

  void Accept(const NodeInspector& inspector) const {
    Inspect(std::addressof(*name), inspector);
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

