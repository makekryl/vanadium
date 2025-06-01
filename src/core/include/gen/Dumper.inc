case NodeKind::Module: {
  const auto* nn = n->As<nodes::Module>();
  DumpGroup("Module", [&] {
    Dump("name", nn->name);
    Dump("language", nn->language);
    Dump("defs", nn->defs);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::Field: {
  const auto* nn = n->As<nodes::Field>();
  DumpGroup("Field", [&] {
    Dump("default_tok", nn->default_tok);
    Dump("type", nn->type);
    Dump("name", nn->name);
    Dump("arraydef", nn->arraydef);
    Dump("pars", nn->pars);
    Dump("value_constraint", nn->value_constraint);
    Dump("length_constraint", nn->length_constraint);
    Dump("optional", nn->optional);
  });
  break;
}
case NodeKind::RefSpec: {
  const auto* nn = n->As<nodes::RefSpec>();
  DumpGroup("RefSpec", [&] {
    Dump("x", nn->x);
  });
  break;
}
case NodeKind::StructSpec: {
  const auto* nn = n->As<nodes::StructSpec>();
  DumpGroup("StructSpec", [&] {
    Dump("kind", nn->kind);
    Dump("fields", nn->fields);
  });
  break;
}
case NodeKind::ListSpec: {
  const auto* nn = n->As<nodes::ListSpec>();
  DumpGroup("ListSpec", [&] {
    Dump("length", nn->length);
    Dump("elemtype", nn->elemtype);
  });
  break;
}
case NodeKind::MapSpec: {
  const auto* nn = n->As<nodes::MapSpec>();
  DumpGroup("MapSpec", [&] {
    Dump("from", nn->from);
    Dump("to", nn->to);
  });
  break;
}
case NodeKind::EnumSpec: {
  const auto* nn = n->As<nodes::EnumSpec>();
  DumpGroup("EnumSpec", [&] {
    Dump("enums", nn->enums);
  });
  break;
}
case NodeKind::BehaviourSpec: {
  const auto* nn = n->As<nodes::BehaviourSpec>();
  DumpGroup("BehaviourSpec", [&] {
    Dump("kind", nn->kind);
    Dump("params", nn->params);
    Dump("runs_on", nn->runs_on);
    Dump("system", nn->system);
    Dump("ret", nn->ret);
  });
  break;
}
case NodeKind::ValueDecl: {
  const auto* nn = n->As<nodes::ValueDecl>();
  DumpGroup("ValueDecl", [&] {
    Dump("kind", nn->kind);
    Dump("template_restriction", nn->template_restriction);
    Dump("modif", nn->modif);
    Dump("type", nn->type);
    Dump("decls", nn->decls);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::Declarator: {
  const auto* nn = n->As<nodes::Declarator>();
  DumpGroup("Declarator", [&] {
    Dump("name", nn->name);
    Dump("arraydef", nn->arraydef);
    Dump("value", nn->value);
  });
  break;
}
case NodeKind::TemplateDecl: {
  const auto* nn = n->As<nodes::TemplateDecl>();
  DumpGroup("TemplateDecl", [&] {
    Dump("restriction", nn->restriction);
    Dump("modif", nn->modif);
    Dump("type", nn->type);
    Dump("name", nn->name);
    Dump("pars", nn->pars);
    Dump("params", nn->params);
    Dump("base", nn->base);
    Dump("value", nn->value);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::ModuleParameterGroup: {
  const auto* nn = n->As<nodes::ModuleParameterGroup>();
  DumpGroup("ModuleParameterGroup", [&] {
    Dump("decls", nn->decls);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::FuncDecl: {
  const auto* nn = n->As<nodes::FuncDecl>();
  DumpGroup("FuncDecl", [&] {
    Dump("external", nn->external);
    Dump("kind", nn->kind);
    Dump("name", nn->name);
    Dump("modif", nn->modif);
    Dump("pars", nn->pars);
    Dump("params", nn->params);
    Dump("runs_on", nn->runs_on);
    Dump("mtc", nn->mtc);
    Dump("system", nn->system);
    Dump("ret", nn->ret);
    Dump("body", nn->body);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::ConstructorDecl: {
  const auto* nn = n->As<nodes::ConstructorDecl>();
  DumpGroup("ConstructorDecl", [&] {
    Dump("params", nn->params);
    Dump("body", nn->body);
  });
  break;
}
case NodeKind::SignatureDecl: {
  const auto* nn = n->As<nodes::SignatureDecl>();
  DumpGroup("SignatureDecl", [&] {
    Dump("name", nn->name);
    Dump("pars", nn->pars);
    Dump("params", nn->params);
    Dump("noblock", nn->noblock);
    Dump("ret", nn->ret);
    Dump("exception", nn->exception);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::BlockStmt: {
  const auto* nn = n->As<nodes::BlockStmt>();
  DumpGroup("BlockStmt", [&] {
    Dump("stmts", nn->stmts);
  });
  break;
}
case NodeKind::BranchStmt: {
  const auto* nn = n->As<nodes::BranchStmt>();
  DumpGroup("BranchStmt", [&] {
    Dump("kind", nn->kind);
    Dump("label", nn->label);
  });
  break;
}
case NodeKind::ReturnStmt: {
  const auto* nn = n->As<nodes::ReturnStmt>();
  DumpGroup("ReturnStmt", [&] {
    Dump("result", nn->result);
  });
  break;
}
case NodeKind::AltStmt: {
  const auto* nn = n->As<nodes::AltStmt>();
  DumpGroup("AltStmt", [&] {
    Dump("kind", nn->kind);
    Dump("no_default", nn->no_default);
    Dump("body", nn->body);
  });
  break;
}
case NodeKind::CallStmt: {
  const auto* nn = n->As<nodes::CallStmt>();
  DumpGroup("CallStmt", [&] {
    Dump("stmt", nn->stmt);
    Dump("body", nn->body);
  });
  break;
}
case NodeKind::ForStmt: {
  const auto* nn = n->As<nodes::ForStmt>();
  DumpGroup("ForStmt", [&] {
    Dump("init", nn->init);
    Dump("cond", nn->cond);
    Dump("post", nn->post);
    Dump("body", nn->body);
  });
  break;
}
case NodeKind::ForRangeStmt: {
  const auto* nn = n->As<nodes::ForRangeStmt>();
  DumpGroup("ForRangeStmt", [&] {
    Dump("init", nn->init);
    Dump("range", nn->range);
    Dump("body", nn->body);
  });
  break;
}
case NodeKind::WhileStmt: {
  const auto* nn = n->As<nodes::WhileStmt>();
  DumpGroup("WhileStmt", [&] {
    Dump("cond", nn->cond);
    Dump("body", nn->body);
  });
  break;
}
case NodeKind::DoWhileStmt: {
  const auto* nn = n->As<nodes::DoWhileStmt>();
  DumpGroup("DoWhileStmt", [&] {
    Dump("body", nn->body);
    Dump("cond", nn->cond);
  });
  break;
}
case NodeKind::IfStmt: {
  const auto* nn = n->As<nodes::IfStmt>();
  DumpGroup("IfStmt", [&] {
    Dump("cond", nn->cond);
    Dump("consequent", nn->consequent);
    Dump("alternate", nn->alternate);
  });
  break;
}
case NodeKind::SelectStmt: {
  const auto* nn = n->As<nodes::SelectStmt>();
  DumpGroup("SelectStmt", [&] {
    Dump("is_union", nn->is_union);
    Dump("tag", nn->tag);
    Dump("clauses", nn->clauses);
  });
  break;
}
case NodeKind::CaseClause: {
  const auto* nn = n->As<nodes::CaseClause>();
  DumpGroup("CaseClause", [&] {
    Dump("cond", nn->cond);
    Dump("body", nn->body);
  });
  break;
}
case NodeKind::CommClause: {
  const auto* nn = n->As<nodes::CommClause>();
  DumpGroup("CommClause", [&] {
    Dump("is_else", nn->is_else);
    Dump("x", nn->x);
    Dump("comm", nn->comm);
    Dump("body", nn->body);
  });
  break;
}
case NodeKind::LanguageSpec: {
  const auto* nn = n->As<nodes::LanguageSpec>();
  DumpGroup("LanguageSpec", [&] {
    Dump("list", nn->list);
  });
  break;
}
case NodeKind::ModuleDef: {
  const auto* nn = n->As<nodes::ModuleDef>();
  DumpGroup("ModuleDef", [&] {
    Dump("visibility", nn->visibility);
    Dump("def", nn->def);
  });
  break;
}
case NodeKind::WithSpec: {
  const auto* nn = n->As<nodes::WithSpec>();
  DumpGroup("WithSpec", [&] {
    Dump("list", nn->list);
  });
  break;
}
case NodeKind::WithStmt: {
  const auto* nn = n->As<nodes::WithStmt>();
  DumpGroup("WithStmt", [&] {
    Dump("kind", nn->kind);
    Dump("overrides", nn->overrides);
    Dump("list", nn->list);
    Dump("value", nn->value);
  });
  break;
}
case NodeKind::ValueLiteral: {
  const auto* nn = n->As<nodes::ValueLiteral>();
  DumpGroup("ValueLiteral", [&] {
    Dump("tok", nn->tok);
  });
  break;
}
case NodeKind::SelectorExpr: {
  const auto* nn = n->As<nodes::SelectorExpr>();
  DumpGroup("SelectorExpr", [&] {
    Dump("x", nn->x);
    Dump("sel", nn->sel);
  });
  break;
}
case NodeKind::DefKindExpr: {
  const auto* nn = n->As<nodes::DefKindExpr>();
  DumpGroup("DefKindExpr", [&] {
    Dump("kind", nn->kind);
    Dump("list", nn->list);
  });
  break;
}
case NodeKind::ExceptExpr: {
  const auto* nn = n->As<nodes::ExceptExpr>();
  DumpGroup("ExceptExpr", [&] {
    Dump("x", nn->x);
    Dump("list", nn->list);
  });
  break;
}
case NodeKind::FromExpr: {
  const auto* nn = n->As<nodes::FromExpr>();
  DumpGroup("FromExpr", [&] {
    Dump("kind", nn->kind);
    Dump("from", nn->from);
    Dump("x", nn->x);
  });
  break;
}
case NodeKind::ModifiesExpr: {
  const auto* nn = n->As<nodes::ModifiesExpr>();
  DumpGroup("ModifiesExpr", [&] {
    Dump("x", nn->x);
    Dump("y", nn->y);
  });
  break;
}
case NodeKind::ParenExpr: {
  const auto* nn = n->As<nodes::ParenExpr>();
  DumpGroup("ParenExpr", [&] {
    Dump("list", nn->list);
  });
  break;
}
case NodeKind::PostExpr: {
  const auto* nn = n->As<nodes::PostExpr>();
  DumpGroup("PostExpr", [&] {
    Dump("x", nn->x);
    Dump("op", nn->op);
  });
  break;
}
case NodeKind::BinaryExpr: {
  const auto* nn = n->As<nodes::BinaryExpr>();
  DumpGroup("BinaryExpr", [&] {
    Dump("x", nn->x);
    Dump("op", nn->op);
    Dump("y", nn->y);
  });
  break;
}
case NodeKind::UnaryExpr: {
  const auto* nn = n->As<nodes::UnaryExpr>();
  DumpGroup("UnaryExpr", [&] {
    Dump("op", nn->op);
    Dump("x", nn->x);
  });
  break;
}
case NodeKind::ValueExpr: {
  const auto* nn = n->As<nodes::ValueExpr>();
  DumpGroup("ValueExpr", [&] {
    Dump("x", nn->x);
    Dump("y", nn->y);
  });
  break;
}
case NodeKind::ParamExpr: {
  const auto* nn = n->As<nodes::ParamExpr>();
  DumpGroup("ParamExpr", [&] {
    Dump("x", nn->x);
    Dump("y", nn->y);
  });
  break;
}
case NodeKind::ImportDecl: {
  const auto* nn = n->As<nodes::ImportDecl>();
  DumpGroup("ImportDecl", [&] {
    Dump("module", nn->module);
    Dump("language", nn->language);
    Dump("list", nn->list);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::GroupDecl: {
  const auto* nn = n->As<nodes::GroupDecl>();
  DumpGroup("GroupDecl", [&] {
    Dump("name", nn->name);
    Dump("defs", nn->defs);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::FriendDecl: {
  const auto* nn = n->As<nodes::FriendDecl>();
  DumpGroup("FriendDecl", [&] {
    Dump("module", nn->module);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::SubTypeDecl: {
  const auto* nn = n->As<nodes::SubTypeDecl>();
  DumpGroup("SubTypeDecl", [&] {
    Dump("field", nn->field);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::StructTypeDecl: {
  const auto* nn = n->As<nodes::StructTypeDecl>();
  DumpGroup("StructTypeDecl", [&] {
    Dump("kind", nn->kind);
    Dump("name", nn->name);
    Dump("pars", nn->pars);
    Dump("fields", nn->fields);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::ClassTypeDecl: {
  const auto* nn = n->As<nodes::ClassTypeDecl>();
  DumpGroup("ClassTypeDecl", [&] {
    Dump("modif", nn->modif);
    Dump("name", nn->name);
    Dump("extends", nn->extends);
    Dump("runs_on", nn->runs_on);
    Dump("mtc", nn->mtc);
    Dump("system", nn->system);
    Dump("defs", nn->defs);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::MapTypeDecl: {
  const auto* nn = n->As<nodes::MapTypeDecl>();
  DumpGroup("MapTypeDecl", [&] {
    Dump("spec", nn->spec);
    Dump("name", nn->name);
    Dump("pars", nn->pars);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::EnumTypeDecl: {
  const auto* nn = n->As<nodes::EnumTypeDecl>();
  DumpGroup("EnumTypeDecl", [&] {
    Dump("name", nn->name);
    Dump("pars", nn->pars);
    Dump("enums", nn->enums);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::BehaviourTypeDecl: {
  const auto* nn = n->As<nodes::BehaviourTypeDecl>();
  DumpGroup("BehaviourTypeDecl", [&] {
    Dump("kind", nn->kind);
    Dump("name", nn->name);
    Dump("pars", nn->pars);
    Dump("params", nn->params);
    Dump("runs_on", nn->runs_on);
    Dump("system", nn->system);
    Dump("ret", nn->ret);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::PortTypeDecl: {
  const auto* nn = n->As<nodes::PortTypeDecl>();
  DumpGroup("PortTypeDecl", [&] {
    Dump("name", nn->name);
    Dump("pars", nn->pars);
    Dump("realtime", nn->realtime);
    Dump("attrs", nn->attrs);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::PortAttribute: {
  const auto* nn = n->As<nodes::PortAttribute>();
  DumpGroup("PortAttribute", [&] {
    Dump("kind", nn->kind);
    Dump("types", nn->types);
  });
  break;
}
case NodeKind::PortMapAttribute: {
  const auto* nn = n->As<nodes::PortMapAttribute>();
  DumpGroup("PortMapAttribute", [&] {
    Dump("kind", nn->kind);
    Dump("params", nn->params);
  });
  break;
}
case NodeKind::ComponentTypeDecl: {
  const auto* nn = n->As<nodes::ComponentTypeDecl>();
  DumpGroup("ComponentTypeDecl", [&] {
    Dump("name", nn->name);
    Dump("pars", nn->pars);
    Dump("extends", nn->extends);
    Dump("body", nn->body);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::FormalPars: {
  const auto* nn = n->As<nodes::FormalPars>();
  DumpGroup("FormalPars", [&] {
    Dump("list", nn->list);
  });
  break;
}
case NodeKind::FormalPar: {
  const auto* nn = n->As<nodes::FormalPar>();
  DumpGroup("FormalPar", [&] {
    Dump("direction", nn->direction);
    Dump("restriction", nn->restriction);
    Dump("modif", nn->modif);
    Dump("type", nn->type);
    Dump("name", nn->name);
    Dump("arraydef", nn->arraydef);
    Dump("value", nn->value);
  });
  break;
}
case NodeKind::LengthExpr: {
  const auto* nn = n->As<nodes::LengthExpr>();
  DumpGroup("LengthExpr", [&] {
    Dump("x", nn->x);
    Dump("size", nn->size);
  });
  break;
}
case NodeKind::RunsOnSpec: {
  const auto* nn = n->As<nodes::RunsOnSpec>();
  DumpGroup("RunsOnSpec", [&] {
    Dump("comp", nn->comp);
  });
  break;
}
case NodeKind::SystemSpec: {
  const auto* nn = n->As<nodes::SystemSpec>();
  DumpGroup("SystemSpec", [&] {
    Dump("comp", nn->comp);
  });
  break;
}
case NodeKind::MtcSpec: {
  const auto* nn = n->As<nodes::MtcSpec>();
  DumpGroup("MtcSpec", [&] {
    Dump("comp", nn->comp);
  });
  break;
}
case NodeKind::ReturnSpec: {
  const auto* nn = n->As<nodes::ReturnSpec>();
  DumpGroup("ReturnSpec", [&] {
    Dump("restriction", nn->restriction);
    Dump("modif", nn->modif);
    Dump("type", nn->type);
  });
  break;
}
case NodeKind::RestrictionSpec: {
  const auto* nn = n->As<nodes::RestrictionSpec>();
  DumpGroup("RestrictionSpec", [&] {
    Dump("is_template", nn->is_template);
    Dump("type", nn->type);
  });
  break;
}
case NodeKind::IndexExpr: {
  const auto* nn = n->As<nodes::IndexExpr>();
  DumpGroup("IndexExpr", [&] {
    Dump("x", nn->x);
    Dump("index", nn->index);
  });
  break;
}
case NodeKind::CallExpr: {
  const auto* nn = n->As<nodes::CallExpr>();
  DumpGroup("CallExpr", [&] {
    Dump("fun", nn->fun);
    Dump("args", nn->args);
  });
  break;
}
case NodeKind::RedirectExpr: {
  const auto* nn = n->As<nodes::RedirectExpr>();
  DumpGroup("RedirectExpr", [&] {
    Dump("x", nn->x);
    Dump("value", nn->value);
    Dump("param", nn->param);
    Dump("sender", nn->sender);
    Dump("to_index", nn->to_index);
    Dump("timestamp", nn->timestamp);
  });
  break;
}
case NodeKind::RedirectToIndex: {
  const auto* nn = n->As<nodes::RedirectToIndex>();
  DumpGroup("RedirectToIndex", [&] {
    Dump("value", nn->value);
    Dump("index", nn->index);
  });
  break;
}
case NodeKind::ParametrizedIdent: {
  const auto* nn = n->As<nodes::ParametrizedIdent>();
  DumpGroup("ParametrizedIdent", [&] {
    Dump("ident", nn->ident);
    Dump("params", nn->params);
  });
  break;
}
case NodeKind::CompositeLiteral: {
  const auto* nn = n->As<nodes::CompositeLiteral>();
  DumpGroup("CompositeLiteral", [&] {
    Dump("list", nn->list);
  });
  break;
}
case NodeKind::RegexpExpr: {
  const auto* nn = n->As<nodes::RegexpExpr>();
  DumpGroup("RegexpExpr", [&] {
    Dump("nocase", nn->nocase);
    Dump("x", nn->x);
  });
  break;
}
case NodeKind::PatternExpr: {
  const auto* nn = n->As<nodes::PatternExpr>();
  DumpGroup("PatternExpr", [&] {
    Dump("nocase", nn->nocase);
    Dump("x", nn->x);
  });
  break;
}
case NodeKind::DecodedExpr: {
  const auto* nn = n->As<nodes::DecodedExpr>();
  DumpGroup("DecodedExpr", [&] {
    Dump("params", nn->params);
    Dump("x", nn->x);
  });
  break;
}
case NodeKind::DynamicExpr: {
  const auto* nn = n->As<nodes::DynamicExpr>();
  DumpGroup("DynamicExpr", [&] {
    Dump("body", nn->body);
  });
  break;
}
case NodeKind::DecmatchExpr: {
  const auto* nn = n->As<nodes::DecmatchExpr>();
  DumpGroup("DecmatchExpr", [&] {
    Dump("params", nn->params);
    Dump("x", nn->x);
  });
  break;
}
case NodeKind::ControlPart: {
  const auto* nn = n->As<nodes::ControlPart>();
  DumpGroup("ControlPart", [&] {
    Dump("body", nn->body);
    Dump("with", nn->with);
  });
  break;
}
case NodeKind::AssignmentExpr: {
  const auto* nn = n->As<nodes::AssignmentExpr>();
  DumpGroup("AssignmentExpr", [&] {
    Dump("property", nn->property);
    Dump("value", nn->value);
  });
  break;
}
