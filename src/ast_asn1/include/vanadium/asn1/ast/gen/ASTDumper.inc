case NodeKind::Module: {
  const auto* nn = n->As<nodes::Module>();
  DumpGroup("Module", [&] {
    Dump("name", nn->name);
    Dump("components", nn->components);
    Dump("tags", nn->tags);
    Dump("extensibility", nn->extensibility);
    Dump("exports", nn->exports);
    Dump("imports", nn->imports);
    Dump("defs", nn->defs);
  });
  break;
}
case NodeKind::DefinitiveComponents: {
  const auto* nn = n->As<nodes::DefinitiveComponents>();
  DumpGroup("DefinitiveComponents", [&] {
    Dump("list", nn->list);
  });
  break;
}
case NodeKind::DefinitiveComponent: {
  const auto* nn = n->As<nodes::DefinitiveComponent>();
  DumpGroup("DefinitiveComponent", [&] {
    Dump("name", nn->name);
    Dump("number", nn->number);
  });
  break;
}
case NodeKind::ModuleExports: {
  const auto* nn = n->As<nodes::ModuleExports>();
  DumpGroup("ModuleExports", [&] {
    Dump("all", nn->all);
    Dump("list", nn->list);
  });
  break;
}
case NodeKind::ModuleImports: {
  const auto* nn = n->As<nodes::ModuleImports>();
  DumpGroup("ModuleImports", [&] {
    Dump("list", nn->list);
  });
  break;
}
case NodeKind::SymbolsFromModule: {
  const auto* nn = n->As<nodes::SymbolsFromModule>();
  DumpGroup("SymbolsFromModule", [&] {
    Dump("provider", nn->provider);
    Dump("list", nn->list);
  });
  break;
}
case NodeKind::SymbolReference: {
  const auto* nn = n->As<nodes::SymbolReference>();
  DumpGroup("SymbolReference", [&] {
    Dump("ident", nn->ident);
    Dump("parametrized", nn->parametrized);
  });
  break;
}
