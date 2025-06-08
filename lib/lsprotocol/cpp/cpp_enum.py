import generator.model as model

from .builder import SourceCodeBuilder, TypeEntry, TypesRegistry
from .cpp_utils import as_enum_v, lines_to_comment, lines_to_multiline_comment


def _generate_enum_serialization(
  enum: model.Enum,
) -> SourceCodeBuilder:
  buf = SourceCodeBuilder()

  buf.write("template <>")
  buf.write(f"void detail::SerializeEnum({enum.name} val, ryml::NodeRef n) {{")
  with buf.indented():
    buf.write("switch (val) {")
    with buf.indented():
      for item in enum.values:
        buf.write(
          f'case {enum.name}::{as_enum_v(item.name)}: n = "{item.value}"; break;'
        )
    buf.write("}")
  buf.write("}")

  buf.newline()

  buf.write("template <>")
  buf.write(f"{enum.name} detail::DeserializeEnum(ryml::ConstNodeRef n) {{")
  with buf.indented():
    buf.write("const auto tv = n.val();")
    for item in enum.values:
      buf.write(
        f'if ("{item.value}" == tv) {{ return {enum.name}::{as_enum_v(item.name)}; }}'
      )
    buf.write("assert(false);")
    buf.write("return {};")
  buf.write("}")

  buf.newline()

  return buf


def generate_enum(enum: model.Enum) -> TypeEntry:
  is_int = all(isinstance(item.value, int) for item in enum.values)
  # TODO: set enum base type

  buf = SourceCodeBuilder()
  if enum.documentation:
    buf.write_all(
      lines_to_multiline_comment(enum.documentation.splitlines(keepends=False))
    )

  buf.write(f"enum class {enum.name} {{")
  with buf.indented():
    for i, item in enumerate(enum.values):
      if item.documentation:
        if i != 0:
          buf.newline()
        buf.write_all(lines_to_comment(item.documentation.splitlines(keepends=False)))
      val = f" = {item.value}" if is_int else ""
      buf.write(f"{as_enum_v(item.name)}{val},")

  buf.write("};")
  buf.newline()

  return TypeEntry(
    header=buf,
    implementation=_generate_enum_serialization(enum),
  )


def generate_enums(spec: model.LSPModel) -> TypesRegistry:
  reg = TypesRegistry()
  for enum in spec.enumerations:
    reg[enum.name] = generate_enum(enum)
  return reg
