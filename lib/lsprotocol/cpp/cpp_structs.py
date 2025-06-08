import generator.model as model

from .builder import SourceCodeBuilder, TypeEntry, TypesRegistry
from .cpp_commons import (
  get_type_name,
  get_used_types_names,
)
from .cpp_unions import generate_union
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
  src_buf = SourceCodeBuilder()

  if struct.documentation:
    hdr_buf.write_all(
      lines_to_multiline_comment(struct.documentation.splitlines(keepends=False))
    )

  hdr_buf.write(f"struct {struct.name} : detail::StructWrapper {{")
  with hdr_buf.indented():
    hdr_buf.write("using StructWrapper::StructWrapper;")
    hdr_buf.newline()

    mandatory_inner_structs: list[str] = []
    skipped = set[str]()
    i = 0
    for prop in list(struct.properties) + _get_extended_props(struct, spec):
      if prop.type.kind == "stringLiteral":
        skipped.add(prop.name)

      if prop.name in skipped:
        assert not prop.optional
        continue

      if prop.documentation:
        if i != 0:
          hdr_buf.newline()
        hdr_buf.write_all(
          lines_to_comment(prop.documentation.splitlines(keepends=False))
        )

      typename = get_type_name(prop.type, prop.optional)

      is_enum = prop.type.kind == "reference" and any(
        x.name == prop.type.name for x in spec.enumerations
      )
      alias_for = (
        next(filter(lambda x: x.name == prop.type.name, spec.typeAliases), None)
        if prop.type.kind == "reference"
        else None
      )

      actual_type = prop.type
      if alias_for:
        actual_type = alias_for.type

      getter = "{0}"
      match actual_type.kind:
        case "reference":
          if is_enum:
            getter = f"detail::DeserializeEnum<{prop.type.name}>({{0}})"
        case "base":
          getter = f"detail::NodeAsPrimitive<{typename}>({{0}})"
        case "or":
          typename = f"Union{i}"
          generate_union(
            name=typename,
            typedef=actual_type,
            spec=spec,
            hdr_buf=hdr_buf,
            src_buf=src_buf,
            wrapped_in=struct.name,
          )
          typename = f"{struct.name}::{typename}"
        case "map" | "array":
          pass
        case _:
          raise ValueError(f"Unknown base type: {prop.type.kind}")

      json_node_object = f'n_.find_child("{prop.name}")'

      hdr_buf.write(f"[[nodiscard]] {typename} {prop.name}();")
      src_buf.write(
        f"{typename} {struct.name}::{prop.name}() {{ return {getter.format(json_node_object)}; }}"
      )

      add_child = f'n_.append_child() << ryml::key("{prop.name}")'

      if prop.optional:
        hdr_buf.write(f"[[nodiscard]] bool has_{prop.name}() const;")
        src_buf.write(
          f'bool {struct.name}::has_{prop.name}() const {{ return n_.has_child("{prop.name}"); }}'
        )
      elif not actual_type.kind == "base" and not is_enum:
        mandatory_inner_structs.append((typename, add_child))

      if actual_type.kind == "base" or is_enum:
        setter = (
          "detail::SerializeEnum(val, {0})"
          if is_enum
          else "detail::SetPrimitive({0}, val)"
        )
        hdr_buf.write(f"void set_{prop.name}({typename} val);")
        src_buf.write(
          f"void {struct.name}::set_{prop.name}({typename} val) {{ auto c = {json_node_object}; if (c.invalid()) {{ c = {add_child}; }} {setter.format('c')}; }}"
        )
      elif prop.optional:
        hdr_buf.write(f"[[nodiscard]] {typename} add_{prop.name}();")
        src_buf.write(
          f"{typename} {struct.name}::add_{prop.name}() {{ auto w = {typename}({getter.format(add_child)}); w.init(); return w; }}"
        )

      i += 1

    hdr_buf.newline()
    hdr_buf.write("void init();")
    src_buf.write(
      f"void {struct.name}::init() {{ StructWrapper::init(); {' '.join([f'{t}({ac}).init();' for t, ac in mandatory_inner_structs])} }}"
    )
  hdr_buf.write("};")
  hdr_buf.newline()

  return TypeEntry(
    header=hdr_buf,
    implementation=src_buf,
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
