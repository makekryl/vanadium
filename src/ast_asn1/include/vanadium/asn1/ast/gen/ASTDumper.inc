case NodeKind::Ident: {
  const auto* nn = n->As<nodes::Ident>();
  DumpGroup("Ident", [&] {
  });
  break;
}
case NodeKind::Module: {
  const auto* nn = n->As<nodes::Module>();
  DumpGroup("Module", [&] {
    Dump("name", nn->name);
    Dump("oidComponents", nn->oidComponents);
    Dump("tagDefault", nn->tagDefault);
    Dump("extensibility", nn->extensibility);
    Dump("exports", nn->exports);
    Dump("imports", nn->imports);
    Dump("assignments", nn->assignments);
  });
  break;
}
case NodeKind::ModuleExports: {
  const auto* nn = n->As<nodes::ModuleExports>();
  DumpGroup("ModuleExports", [&] {
    Dump("all", nn->all);
    Dump("symbols", nn->symbols);
  });
  break;
}
case NodeKind::ModuleImports: {
  const auto* nn = n->As<nodes::ModuleImports>();
  DumpGroup("ModuleImports", [&] {
    Dump("symbolsFromModules", nn->symbolsFromModules);
  });
  break;
}
case NodeKind::SymbolsFromModule: {
  const auto* nn = n->As<nodes::SymbolsFromModule>();
  DumpGroup("SymbolsFromModule", [&] {
    Dump("symbols", nn->symbols);
    Dump("fromModule", nn->fromModule);
  });
  break;
}
case NodeKind::Symbol: {
  const auto* nn = n->As<nodes::Symbol>();
  DumpGroup("Symbol", [&] {
    Dump("ident", nn->ident);
    Dump("parametrized", nn->parametrized);
  });
  break;
}
case NodeKind::Assignment: {
  const auto* nn = n->As<nodes::Assignment>();
  DumpGroup("Assignment", [&] {
    Dump("name", nn->name);
    Dump("rhs", nn->rhs);
  });
  break;
}
case NodeKind::TypeAssignment: {
  const auto* nn = n->As<nodes::TypeAssignment>();
  DumpGroup("TypeAssignment", [&] {
    Dump("type", nn->type);
  });
  break;
}
case NodeKind::ValueAssignment: {
  const auto* nn = n->As<nodes::ValueAssignment>();
  DumpGroup("ValueAssignment", [&] {
    Dump("type", nn->type);
    Dump("value", nn->value);
  });
  break;
}
case NodeKind::ParameterizedAssignment: {
  const auto* nn = n->As<nodes::ParameterizedAssignment>();
  DumpGroup("ParameterizedAssignment", [&] {
    Dump("parameters", nn->parameters);
    Dump("rhs", nn->rhs);
  });
  break;
}
case NodeKind::ObjectClassAssignment: {
  const auto* nn = n->As<nodes::ObjectClassAssignment>();
  DumpGroup("ObjectClassAssignment", [&] {
    Dump("objectClass", nn->objectClass);
  });
  break;
}
case NodeKind::Tag: {
  const auto* nn = n->As<nodes::Tag>();
  DumpGroup("Tag", [&] {
    Dump("tagClass", nn->tagClass);
    Dump("number", nn->number);
    Dump("mode", nn->mode);
  });
  break;
}
case NodeKind::BuiltinTypeIdent: {
  const auto* nn = n->As<nodes::BuiltinTypeIdent>();
  DumpGroup("BuiltinTypeIdent", [&] {
    Dump("tok1", nn->tok1);
    Dump("tok2", nn->tok2);
  });
  break;
}
case NodeKind::AsnType: {
  const auto* nn = n->As<nodes::AsnType>();
  DumpGroup("AsnType", [&] {
    Dump("tag", nn->tag);
    Dump("base", nn->base);
    Dump("constraints", nn->constraints);
  });
  break;
}
case NodeKind::DefinedType: {
  const auto* nn = n->As<nodes::DefinedType>();
  DumpGroup("DefinedType", [&] {
    Dump("name", nn->name);
    Dump("module", nn->module);
    Dump("parameters", nn->parameters);
  });
  break;
}
case NodeKind::SequenceType: {
  const auto* nn = n->As<nodes::SequenceType>();
  DumpGroup("SequenceType", [&] {
    Dump("components", nn->components);
    Dump("extensible", nn->extensible);
  });
  break;
}
case NodeKind::SetType: {
  const auto* nn = n->As<nodes::SetType>();
  DumpGroup("SetType", [&] {
    Dump("components", nn->components);
    Dump("extensible", nn->extensible);
  });
  break;
}
case NodeKind::ComponentType: {
  const auto* nn = n->As<nodes::ComponentType>();
  DumpGroup("ComponentType", [&] {
    Dump("content", nn->content);
    Dump("optional", nn->optional);
    Dump("defaultValue", nn->defaultValue);
  });
  break;
}
case NodeKind::SequenceOfType: {
  const auto* nn = n->As<nodes::SequenceOfType>();
  DumpGroup("SequenceOfType", [&] {
    Dump("elementType", nn->elementType);
    Dump("sizeConstraint", nn->sizeConstraint);
  });
  break;
}
case NodeKind::SetOfType: {
  const auto* nn = n->As<nodes::SetOfType>();
  DumpGroup("SetOfType", [&] {
    Dump("elementType", nn->elementType);
    Dump("sizeConstraint", nn->sizeConstraint);
  });
  break;
}
case NodeKind::ChoiceType: {
  const auto* nn = n->As<nodes::ChoiceType>();
  DumpGroup("ChoiceType", [&] {
    Dump("alternatives", nn->alternatives);
    Dump("extensionAlternatives", nn->extensionAlternatives);
  });
  break;
}
case NodeKind::EnumeratedType: {
  const auto* nn = n->As<nodes::EnumeratedType>();
  DumpGroup("EnumeratedType", [&] {
    Dump("items", nn->items);
    Dump("extensible", nn->extensible);
  });
  break;
}
case NodeKind::EnumerationItem: {
  const auto* nn = n->As<nodes::EnumerationItem>();
  DumpGroup("EnumerationItem", [&] {
    Dump("value", nn->value);
  });
  break;
}
case NodeKind::NamedNumber: {
  const auto* nn = n->As<nodes::NamedNumber>();
  DumpGroup("NamedNumber", [&] {
    Dump("name", nn->name);
    Dump("value", nn->value);
  });
  break;
}
case NodeKind::NamedType: {
  const auto* nn = n->As<nodes::NamedType>();
  DumpGroup("NamedType", [&] {
    Dump("name", nn->name);
    Dump("type", nn->type);
  });
  break;
}
case NodeKind::Constraint: {
  const auto* nn = n->As<nodes::Constraint>();
  DumpGroup("Constraint", [&] {
    Dump("spec", nn->spec);
    Dump("exception", nn->exception);
  });
  break;
}
case NodeKind::ExceptionSpec: {
  const auto* nn = n->As<nodes::ExceptionSpec>();
  DumpGroup("ExceptionSpec", [&] {
    Dump("identification", nn->identification);
  });
  break;
}
case NodeKind::ElementSetSpec: {
  const auto* nn = n->As<nodes::ElementSetSpec>();
  DumpGroup("ElementSetSpec", [&] {
    Dump("root", nn->root);
    Dump("extension", nn->extension);
  });
  break;
}
case NodeKind::ExtensionSpec: {
  const auto* nn = n->As<nodes::ExtensionSpec>();
  DumpGroup("ExtensionSpec", [&] {
    Dump("additionalElements", nn->additionalElements);
  });
  break;
}
case NodeKind::ElementSet: {
  const auto* nn = n->As<nodes::ElementSet>();
  DumpGroup("ElementSet", [&] {
    Dump("unions", nn->unions);
  });
  break;
}
case NodeKind::Intersection: {
  const auto* nn = n->As<nodes::Intersection>();
  DumpGroup("Intersection", [&] {
    Dump("elements", nn->elements);
  });
  break;
}
case NodeKind::Elements: {
  const auto* nn = n->As<nodes::Elements>();
  DumpGroup("Elements", [&] {
    Dump("content", nn->content);
  });
  break;
}
case NodeKind::Value: {
  const auto* nn = n->As<nodes::Value>();
  DumpGroup("Value", [&] {
    Dump("content", nn->content);
  });
  break;
}
case NodeKind::ObjectIdentifierValue: {
  const auto* nn = n->As<nodes::ObjectIdentifierValue>();
  DumpGroup("ObjectIdentifierValue", [&] {
    Dump("components", nn->components);
  });
  break;
}
case NodeKind::OidComponent: {
  const auto* nn = n->As<nodes::OidComponent>();
  DumpGroup("OidComponent", [&] {
    Dump("content", nn->content);
  });
  break;
}
case NodeKind::AssignedOidComponent: {
  const auto* nn = n->As<nodes::AssignedOidComponent>();
  DumpGroup("AssignedOidComponent", [&] {
    Dump("name", nn->name);
    Dump("assignedValue", nn->assignedValue);
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
case NodeKind::DefinedValue: {
  const auto* nn = n->As<nodes::DefinedValue>();
  DumpGroup("DefinedValue", [&] {
    Dump("name", nn->name);
    Dump("module", nn->module);
    Dump("parameters", nn->parameters);
  });
  break;
}
case NodeKind::Parameter: {
  const auto* nn = n->As<nodes::Parameter>();
  DumpGroup("Parameter", [&] {
    Dump("name", nn->name);
    Dump("governor", nn->governor);
  });
  break;
}
case NodeKind::ObjectClass: {
  const auto* nn = n->As<nodes::ObjectClass>();
  DumpGroup("ObjectClass", [&] {
    Dump("definition", nn->definition);
  });
  break;
}
case NodeKind::DefinedObjectClass: {
  const auto* nn = n->As<nodes::DefinedObjectClass>();
  DumpGroup("DefinedObjectClass", [&] {
    Dump("module", nn->module);
    Dump("name", nn->name);
  });
  break;
}
case NodeKind::ObjectClassDefn: {
  const auto* nn = n->As<nodes::ObjectClassDefn>();
  DumpGroup("ObjectClassDefn", [&] {
    Dump("fields", nn->fields);
    Dump("withSyntax", nn->withSyntax);
  });
  break;
}
case NodeKind::Object: {
  const auto* nn = n->As<nodes::Object>();
  DumpGroup("Object", [&] {
    Dump("definition", nn->definition);
  });
  break;
}
case NodeKind::FieldSpec: {
  const auto* nn = n->As<nodes::FieldSpec>();
  DumpGroup("FieldSpec", [&] {
    Dump("name", nn->name);
    Dump("specification", nn->specification);
  });
  break;
}
case NodeKind::ObjectSetSpec: {
  const auto* nn = n->As<nodes::ObjectSetSpec>();
  DumpGroup("ObjectSetSpec", [&] {
    Dump("root", nn->root);
    Dump("extension", nn->extension);
  });
  break;
}
case NodeKind::DefinedObject: {
  const auto* nn = n->As<nodes::DefinedObject>();
  DumpGroup("DefinedObject", [&] {
    Dump("name", nn->name);
  });
  break;
}
case NodeKind::ParameterizedObject: {
  const auto* nn = n->As<nodes::ParameterizedObject>();
  DumpGroup("ParameterizedObject", [&] {
    Dump("object", nn->object);
    Dump("parameters", nn->parameters);
  });
  break;
}
case NodeKind::ActualParameter: {
  const auto* nn = n->As<nodes::ActualParameter>();
  DumpGroup("ActualParameter", [&] {
    Dump("value", nn->value);
  });
  break;
}
case NodeKind::ObjectSet: {
  const auto* nn = n->As<nodes::ObjectSet>();
  DumpGroup("ObjectSet", [&] {
    Dump("spec", nn->spec);
  });
  break;
}
case NodeKind::WithSyntaxSpec: {
  const auto* nn = n->As<nodes::WithSyntaxSpec>();
  DumpGroup("WithSyntaxSpec", [&] {
    Dump("tokens", nn->tokens);
  });
  break;
}
case NodeKind::SyntaxToken: {
  const auto* nn = n->As<nodes::SyntaxToken>();
  DumpGroup("SyntaxToken", [&] {
    Dump("content", nn->content);
  });
  break;
}
case NodeKind::OptionalGroup: {
  const auto* nn = n->As<nodes::OptionalGroup>();
  DumpGroup("OptionalGroup", [&] {
    Dump("tokens", nn->tokens);
  });
  break;
}
