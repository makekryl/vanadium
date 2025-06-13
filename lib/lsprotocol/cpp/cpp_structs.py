import generator.model as model

from .builder import SourceCodeBuilder, TypeEntry, TypesRegistry
from .cpp_commons import get_type_name, get_used_types_names
from .cpp_utils import lines_to_comment, lines_to_multiline_comment


def _generate_struct_decl(struct: model.Structure) -> str:
  inherited_from = [x for x in (struct.extends + struct.mixins)]  # type: ignore
  parenal = (
    f" : {', '.join([parent.name for parent in inherited_from])}"
    if inherited_from
    else ""
  )

  return f"struct {struct.name}{parenal}"


def _generate_struct(struct: model.Structure) -> TypeEntry:
  hdr_buf = SourceCodeBuilder()

  if struct.documentation:
    hdr_buf.write_all(
      lines_to_multiline_comment(struct.documentation.splitlines(keepends=False))
    )

  hdr_buf.write(f"{_generate_struct_decl(struct)} {{")
  hdr_buf.indent()

  for i, prop in enumerate(struct.properties):
    if prop.documentation:
      if i != 0:
        hdr_buf.newline()
      hdr_buf.write_all(lines_to_comment(prop.documentation.splitlines(keepends=False)))
    typename = get_type_name(prop.type, prop.optional)
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
    reg[struct.name] = _generate_struct(struct)
  return reg
