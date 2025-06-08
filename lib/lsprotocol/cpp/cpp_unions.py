from generator import model

from .builder import SourceCodeBuilder
from .cpp_commons import get_type_name, get_union_types


def generate_union(
  name: str,
  typedef: model.LSP_TYPE_SPEC,
  spec: model.LSPModel,
  hdr_buf: SourceCodeBuilder,
  src_buf: SourceCodeBuilder,
  wrapped_in: str = "",
):
  assert typedef.kind == "or"

  full_qualified_name = f"{wrapped_in}::{name}" if wrapped_in else name

  sub_types = [get_type_name(sub_spec) for sub_spec in get_union_types(typedef)]
  hdr_buf.write(
    f"struct {name} : detail::UnionStructWrapper<{', '.join(sub_types)}> {{"
  )
  with hdr_buf.indented():
    hdr_buf.write("using UnionStructWrapper::UnionStructWrapper;")
    hdr_buf.newline()

    for i, alttype in enumerate(get_union_types(typedef)):
      typename = get_type_name(alttype)

      is_enum = alttype.kind == "reference" and any(
        x.name == alttype.name for x in spec.enumerations
      )
      alias_for = (
        next(filter(lambda x: x.name == alttype.name, spec.typeAliases), None)
        if alttype.kind == "reference"
        else None
      )

      actual_type = alttype
      if alias_for:
        actual_type = alias_for.type

      match actual_type.kind:
        case "reference":
          if is_enum:
            getter = f"return detail::DeserializeEnum<{alttype.name}>(n_);"
          else:
            getter = "return {n_};"

          holding_type_name = actual_type.name
        case "base":
          holding_type_name = actual_type.name
          getter = f"return detail::NodeAsPrimitive<{typename}>(n_);"
        case "or":
          generate_union(
            name=f"Union{i}",
            typedef=actual_type,
            spec=spec,
            hdr_buf=hdr_buf,
            src_buf=src_buf,
            wrapped_in=full_qualified_name,
          )
          holding_type_name = f"Union{i}"
          getter = ""
        case "map":
          holding_type_name = f"Map{i}"
          getter = ""
        case "array":
          holding_type_name = actual_type.element.name
          getter = "return {n_};"
        case "literal":
          continue
        case "tuple":
          continue  # TODO
        case _:
          raise ValueError(f"Unknown base type: {alttype.kind}")

      #
      variant_name = f"{holding_type_name}"
      if actual_type.kind not in ["base", "reference"]:
        variant_name = f"{alttype.kind}_{variant_name}"
      #

      hdr_buf.write(f"[[nodiscard]] bool holds_{variant_name}() const;")
      src_buf.write(
        f'bool {full_qualified_name}::holds_{variant_name}() const {{ return n_.has_child("TODO"); }}'
      )

      hdr_buf.write(f"[[nodiscard]] {typename} as_{variant_name}();")
      src_buf.write(
        f"{typename} {full_qualified_name}::as_{variant_name}() {{ {getter} }}"
      )

      if actual_type.kind == "base":
        hdr_buf.write(f"void set_{variant_name}({typename} val);")
        src_buf.write(
          f"void {full_qualified_name}::set_{variant_name}({typename} val) {{ detail::SetPrimitive(n_, val); }}"
        )
      hdr_buf.newline()

  hdr_buf.write("};")
  hdr_buf.newline()
