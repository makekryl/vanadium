import generator.model as model

from .builder import SourceCodeBuilder, TypeEntry, TypesRegistry
from .cpp_commons import get_type_name, get_used_types_names
from .cpp_utils import lines_to_multiline_comment


def _generate_type_alias(alias: model.TypeAlias) -> TypeEntry:
  buf = SourceCodeBuilder()

  if alias.documentation:
    buf.write_all(
      lines_to_multiline_comment(alias.documentation.splitlines(keepends=False))
    )

  buf.write(f"using {alias.name} = {get_type_name(alias.type)};")
  buf.newline()

  return TypeEntry(
    buf=buf,
    dependencies=get_used_types_names(alias.type),
  )


def generate_type_aliases(spec: model.LSPModel) -> TypesRegistry:
  reg = TypesRegistry()
  for alias in spec.typeAliases:
    reg[alias.name] = _generate_type_alias(alias)
  return reg
