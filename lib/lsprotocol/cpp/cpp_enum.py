import generator.model as model

from .builder import SourceCodeBuilder, TypeEntry, TypesRegistry
from .cpp_utils import as_enum_v, lines_to_comment, lines_to_multiline_comment


def generate_enum(enum: model.Enum) -> TypeEntry:
  is_int = all(isinstance(item.value, int) for item in enum.values)
  is_str = all(isinstance(item.value, str) for item in enum.values)

  underlying_type = "std::uint8_t"
  if is_int and any(
    isinstance(item.value, int) and item.value < 0 for item in enum.values
  ):
    underlying_type = (
      "std::int32_t"  # TODO: more generic solution, this targets ErrorCodes enum only
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

  sbuf = None
  if is_str:
    sbuf = SourceCodeBuilder()
    sbuf.write("template<>")
    sbuf.write(f"struct glz::meta<lsp::{enum.name}> {{")
    with sbuf.indented():
      sbuf.write(f"using enum lsp::{enum.name};")
      sbuf.write("static constexpr auto value = enumerate(")
      with sbuf.indented():
        total = len(list(enum.values))
        for i, item in enumerate(enum.values):
          comma = "," if i != (total - 1) else ""
          sbuf.write(f'"{item.value}", {as_enum_v(item.name)}{comma}')
      sbuf.write(");")
    sbuf.write("};")

  return TypeEntry(buf, appendix=sbuf)


def generate_enums(spec: model.LSPModel) -> TypesRegistry:
  reg = TypesRegistry()
  for enum in spec.enumerations:
    if enum.name in ["LanguageKind"]:
      buf = SourceCodeBuilder()
      buf.write(f"using {enum.name} = std::string_view;")
      reg[enum.name] = TypeEntry(buf)
      continue
    reg[enum.name] = generate_enum(enum)
  return reg
