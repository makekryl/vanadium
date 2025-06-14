from generator import model


def lsp_to_base_types(lsp_type: model.BaseType):
  if lsp_type.name in ["string", "RegExp", "DocumentUri", "URI"]:
    return "std::string_view"
  elif lsp_type.name in ["decimal"]:
    return "double"
  elif lsp_type.name in ["integer"]:
    return "std::int32_t"
  elif lsp_type.name in ["uinteger"]:
    return "std::uint32_t"
  elif lsp_type.name in ["boolean"]:
    return "bool"

  raise ValueError(f"Unknown base type: {lsp_type.name}")


def get_used_types_names(type_def: model.LSP_TYPE_SPEC) -> list[str]:
  match type_def.kind:
    case "reference":
      return [type_def.name]  # type: ignore
    case "array":
      return get_used_types_names(type_def.element)  # type: ignore
    case "map":
      return get_used_types_names(type_def.key) + get_used_types_names(  # type: ignore
        type_def=type_def.value  # type: ignore
      )
    case "or" | "tuple":
      sub_set_items = [
        sub_spec
        for sub_spec in type_def.items  # type: ignore
        if not (sub_spec.kind == "base" and sub_spec.name == "null")  # type: ignore
      ]
      return sum([get_used_types_names(sub_spec) for sub_spec in sub_set_items], [])
    case "base" | "literal" | "stringLiteral":
      return []
    case _:
      raise ValueError(f"Unknown type kind: {type_def.kind}")


def get_union_types(typedef: model.LSP_TYPE_SPEC) -> list[model.LSP_TYPE_SPEC]:
  return [
    sub_spec
    for sub_spec in typedef.items  # type: ignore
    if not (sub_spec.kind == "base" and sub_spec.name == "null")  # type: ignore
  ]


def get_type_name(type_def: model.LSP_TYPE_SPEC, optional: bool | None = None) -> str:
  match type_def.kind:
    case "reference":
      name = type_def.name  # type: ignore
    case "array":
      name = f"std::vector<{get_type_name(type_def.element)}>"  # type: ignore
    case "map":
      key_type = get_type_name(type_def.key)  # type: ignore
      value_type = get_type_name(type_def.value)  # type: ignore
      name = f"std::unordered_map<{key_type}, {value_type}>"
    case "base":
      name = lsp_to_base_types(type_def)  # type: ignore
    case "or":
      sub_types = [get_type_name(sub_spec) for sub_spec in get_union_types(type_def)]
      sub_types_str = ", ".join(sub_types)
      if len(sub_types) >= 2:
        name = f"std::variant<{sub_types_str}>"
      elif len(sub_types) == 1:
        name = sub_types[0]
      else:
        raise ValueError(
          f"OR type with more than out of range count of subtypes: {type_def}"
        )
    case "literal":
      name = "LiteralStub"
      # name = generate_literal_struct_type(type_def, name_context) # TODO
    case "stringLiteral":
      name = "const std::string_view"
      # This type in rust requires a custom deserializer that fails if the value is not
      # one of the allowed values. This should be handled by the caller. This cannot be
      # handled here because all this does is handle type names.
    case "tuple":
      # optional = optional or is_special(type_def)
      sub_set_items = [
        sub_spec
        for sub_spec in type_def.items
        if not (sub_spec.kind == "base" and sub_spec.name == "null")
      ]
      sub_types = [get_type_name(sub_spec) for sub_spec in sub_set_items]
      sub_types_str = ", ".join(sub_types)
      if len(sub_types) >= 2:
        name = f"std::tuple<{sub_types_str}>"
      elif len(sub_types) == 1:
        name = sub_types[0]
      else:
        raise ValueError(f"Invalid number of items for tuple: {type_def}")
    case _:
      raise ValueError(f"Unknown type kind: {type_def.kind}")

  return f"std::optional<{name}>" if optional else name  # type: ignore
