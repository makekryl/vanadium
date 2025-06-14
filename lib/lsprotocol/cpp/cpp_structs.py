import generator.model as model

from .builder import SourceCodeBuilder, TypeEntry, TypesRegistry
from .cpp_commons import get_type_name, get_used_types_names
from .cpp_utils import lines_to_comment, lines_to_multiline_comment


def _get_extended_props(
  struct: model.Structure, spec: model.LSPModel
) -> list[model.Property]:
  inherited_from = struct.extends + struct.mixins  # type: ignore
  return sum(
    [
      next(filter(lambda x: x.name == parent.name, spec.structures)).properties
      for parent in inherited_from
    ],
    [],
  )


def _generate_struct(struct: model.Structure, spec: model.LSPModel) -> TypeEntry:
  hdr_buf = SourceCodeBuilder()

  if struct.documentation:
    hdr_buf.write_all(
      lines_to_multiline_comment(struct.documentation.splitlines(keepends=False))
    )

  all_properties: list[model.Property] = list(struct.properties) + _get_extended_props(
    struct, spec
  )

  hdr_buf.write(f"struct {struct.name} {{")
  hdr_buf.indent()

  seen: list[str] = []
  for i, prop in enumerate(all_properties):
    if prop.name in seen:
      continue
    seen.append(prop.name)

    if prop.documentation:
      if i != 0:
        hdr_buf.newline()
      hdr_buf.write_all(lines_to_comment(prop.documentation.splitlines(keepends=False)))
    typename = get_type_name(prop.type, prop.optional)
    if prop.name == "text" and typename == "std::string_view":
      typename = "std::string"  # TODO: glaze sv inplace unescape
    if (
      typename == "std::optional<SelectionRange>" and struct.name == "SelectionRange"
    ):  # TODO: cleanup
      hdr_buf.write(f"SelectionRange* {prop.name}{{nullptr}};")
    elif typename != "LiteralStub":  # TODO: not supported
      hdr_buf.write(f"{typename} {prop.name};")

  hdr_buf.unindent()
  hdr_buf.write("};")
  hdr_buf.newline()

  return TypeEntry(
    buf=hdr_buf,
    dependencies=[
      dep
      for dep in sum(
        [get_used_types_names(prop.type) for prop in struct.properties],
        [x.name for x in (struct.extends + struct.mixins)],  # type: ignore
      )
      if dep != struct.name
    ],
  )


def generate_structs(spec: model.LSPModel) -> TypesRegistry:
  reg = TypesRegistry()
  for struct in spec.structures:
    reg[struct.name] = _generate_struct(struct, spec)
  return reg
