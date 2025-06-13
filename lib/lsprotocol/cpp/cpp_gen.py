import pathlib
from graphlib import TopologicalSorter

import generator.model as model

from .builder import SourceCodeBuilder, TypesRegistry
from .cpp_aliases import generate_type_aliases
from .cpp_enum import generate_enums
from .cpp_structs import generate_structs


def _write_types(reg: TypesRegistry, hdr_buf: SourceCodeBuilder) -> None:
  ordered_types = TopologicalSorter(
    {
      name: entry.dependencies
      for name, entry in reg.items()
      if name not in ["LSPArray", "LSPAny"]
    }
  ).static_order()

  for typename in ordered_types:
    if typename in ["LSPObject", "LSPArray", "LSPAny"]:
      continue
    hdr_buf += reg[typename].buf


def _build_header(reg: TypesRegistry):
  buf = SourceCodeBuilder()
  #
  buf.write("#pragma once")
  buf.newline()
  buf.write("#include <span>")
  buf.write("#include <cstdint>")
  buf.write("#include <tuple>")
  buf.write("#include <string_view>")
  buf.write("#include <optional>")
  buf.write("#include <variant>")
  buf.newline()
  buf.write("// NOLINTBEGIN(readability-identifier-naming)\n")
  buf.write("namespace lsp {")
  buf.newline()
  _write_types(reg, buf)
  buf.newline()
  buf.write("}  // namespace lsp")
  buf.write("\n// NOLINTEND(readability-identifier-naming)")

  return buf.build()


def generate_from_spec(spec: model.LSPModel, output_dir: str, test_dir: str) -> None:
  output_path = pathlib.Path(output_dir)
  if not output_path.exists():
    output_path.mkdir(parents=True, exist_ok=True)

  reg = TypesRegistry()
  reg |= generate_enums(spec)
  reg |= generate_type_aliases(spec)
  reg |= generate_structs(spec)

  (output_path / "include" / "LSProtocol.h").write_text(
    _build_header(reg), encoding="utf-8"
  )
