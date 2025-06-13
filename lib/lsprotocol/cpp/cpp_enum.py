import generator.model as model

from .builder import SourceCodeBuilder, TypeEntry, TypesRegistry
from .cpp_utils import as_enum_v, lines_to_comment, lines_to_multiline_comment


def generate_enum(enum: model.Enum) -> TypeEntry:
  is_int = all(isinstance(item.value, int) for item in enum.values)

  underlying_type = "std::uint8_t"
  if is_int and any(
    isinstance(item.value, int) and item.value < 0 for item in enum.values
  ):
    underlying_type = (
      "std::int16_t"  # TODO: more generic solution, this targets ErrorCodes enum only
    )

  buf = SourceCodeBuilder()
  if enum.documentation:
    buf.write_all(
      lines_to_multiline_comment(enum.documentation.splitlines(keepends=False))
    )

  buf.write(f"enum class {enum.name} : {underlying_type} {{")
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

  return TypeEntry(buf)


def generate_enums(spec: model.LSPModel) -> TypesRegistry:
  reg = TypesRegistry()
  for enum in spec.enumerations:
    reg[enum.name] = generate_enum(enum)
  return reg
