#!/usr/bin/env python3

import argparse
import os
import re
import sys
from contextlib import contextmanager
from dataclasses import dataclass
from graphlib import TopologicalSorter
from pathlib import Path
from typing import Callable, Self, TextIO

import yaml

TAB_WIDTH = 2


@dataclass
class AstNodeField:
  name: str
  typename: str
  indirect: bool  # *
  repeated: bool  # []
  explicitly_optional: bool  # ?

  @property
  def optional(self):
    if self.explicitly_optional:
      return True
    if self.typename in ["Token", "bool"]:
      return False
    return not self.indirect and not self.repeated

  @property
  def is_predefined(self):
    return self.typename in [
      "bool",
      "Token",
      "Ident",
      "Node",
      "Expr",
      "Stmt",
      "TypeSpec",
    ]


@dataclass
class AstNode:
  name: str
  fields: list[AstNodeField]

  @property
  def basic_type(self):
    if self.name in [
      "ParametrizedIdent",
    ]:
      return "Ident"
    if self.name.endswith("Expr") or self.name in [
      "ValueLiteral",
      "CompositeLiteral",
    ]:
      return "Expr"
    if self.name.endswith("Decl") or self.name in ["ModuleParameterGroup"]:
      return "Decl"
    if self.name.endswith("Stmt") or self.name in ["CommClause"]:
      return "Stmt"
    if self.name.endswith("Spec") or self.name in ["Kind"]:
      return "TypeSpec"
    return "Node"


AstNodesDict = dict[str, AstNode]


def parse_nodes(stream: TextIO) -> AstNodesDict:
  data = yaml.load(stream, Loader=yaml.Loader)

  def parse_field(node: str, name: str, typedesc: str) -> AstNodeField:
    match = re.search(r"^(\b[A-Za-z].*?\b)(\*)?(\[\])?(\?)?$", typedesc)
    if not match:
      raise ValueError(f"Invalid type description for '{node}.{name}'")

    return AstNodeField(
      name=name,
      typename=match.group(1),
      indirect=match.group(2) is not None,
      repeated=match.group(3) is not None,
      explicitly_optional=match.group(4) is not None,
    )

  return {
    node: AstNode(
      name=node,
      fields=[
        parse_field(node=node, name=fieldname, typedesc=fieldtype)
        for fieldname, fieldtype in fields.items()
      ],
    )
    for node, fields in data.items()
  }


class SourceCodeBuilder:
  __TAB_WIDTH = 2

  def __init__(self):
    self.__buf = ""
    self.__indentation = 0

  def indent(self):
    self.__indentation += 1

  def unindent(self):
    self.__indentation -= 1

  @contextmanager
  def indented(self):
    self.indent()
    try:
      yield
    finally:
      self.unindent()

  def write(self, s: str):
    self.__buf += (self.__indentation * SourceCodeBuilder.__TAB_WIDTH * " ") + s + "\n"

  def write_all(self, lines: list[str]):
    for line in lines:
      self.write(line)

  def newline(self):
    self.__buf += "\n"

  def spinoff(self, delta: int):
    builder = SourceCodeBuilder()
    builder.__indentation = self.__indentation + delta
    return builder

  def empty(self) -> bool:
    return len(self.__buf) == 0

  def __iadd__(self, other: Self) -> Self:
    self.__buf += other.__buf
    return self

  def build(self):
    return self.__buf


def generate_node_inspecting_code(
  node: AstNode, inspector_obj: str, buf: SourceCodeBuilder
):
  def emit_inspect(nodeptr: str):
    buf.write(f"Inspect({nodeptr}, {inspector_obj});")

  for field in node.fields:
    if field.typename in ["bool", "Token"]:
      continue

    variable = field.name

    if field.repeated:
      variable = "n"
      buf.write(f"for (const auto* {variable} : {field.name}) {{")
      buf.indent()

    if field.indirect:
      if field.optional:
        prediction = " [[likely]]" if field.name in ["params"] else ""
        buf.write(f"if ({variable} != nullptr){prediction} {{")
        buf.indent()
      emit_inspect(variable)
      if field.optional:
        buf.unindent()
        buf.write("}")
    else:
      if field.optional or not field.repeated:
        buf.write(f"if ({variable}.has_value()) {{")
        with buf.indented():
          emit_inspect(f"std::addressof(*{variable})")
        buf.write("}")
      else:
        emit_inspect(variable)

    if field.repeated:
      buf.unindent()
      buf.write("}")

  return buf


def generate_nodes_descriptors(nodes: AstNodesDict) -> str:
  ordered_nodes = TopologicalSorter(
    {
      node.name: [field.typename for field in node.fields if not field.is_predefined]
      for node in nodes.values()
    }
  ).static_order()

  buf = SourceCodeBuilder()

  for name in ordered_nodes:
    node = nodes[name]
    buf.write(f"struct {name} : {node.basic_type} {{")
    with buf.indented():
      buf.write(f"{name}() : {node.basic_type}(NodeKind::{name}) {{}}")
      buf.newline()

      for field in node.fields:
        stored_type = field.typename
        if field.indirect:
          stored_type = f"{stored_type}*"
        if field.repeated:
          stored_type = f"std::vector<{stored_type}>"  # TODO: span on arena
        if not field.indirect and field.optional:
          stored_type = f"std::optional<{stored_type}>"

        initializer = ""
        if field.optional and field.indirect and not field.repeated:
          initializer = "{nullptr}"
        elif field.typename == "bool":
          initializer = "{false}"

        buf.write(f"{stored_type} {field.name}{initializer};")

      buf.newline()

      inspecting_code_buf = buf.spinoff(delta=1)
      inspector_obj = "inspector"
      generate_node_inspecting_code(node, inspector_obj, inspecting_code_buf)
      buf.write(
        f"void Accept(const NodeInspector&{'' if inspecting_code_buf.empty() else f' {inspector_obj}'}) const {{"
      )
      with buf.indented():
        buf += inspecting_code_buf
      buf.write("}")
    buf.write("};")
    buf.newline()

  return buf.build()


def generate_macro_inspector(nodes: AstNodesDict) -> str:
  buf = SourceCodeBuilder()
  for nodename in nodes.keys():
    buf.write(f"FORWARD_ACCEPT({nodename})")
  return buf.build()


def generate_dumper_code(nodes: AstNodesDict) -> str:
  buf = SourceCodeBuilder()

  for node in nodes.values():
    buf.write(f"case NodeKind::{node.name}: {{")
    with buf.indented():
      buf.write(f"const auto* nn = n->As<nodes::{node.name}>();")
      buf.write(f'DumpGroup("{node.name}", [&] {{')
      with buf.indented():
        for field in node.fields:
          buf.write(f'Dump("{field.name}", nn->{field.name});')
      buf.write("});")
      buf.write("break;")
    buf.write("}")

  return buf.build()


class ArgumentsNamespace(argparse.Namespace):
  input: str
  output: str


def main():
  parser = argparse.ArgumentParser()

  def file_path(s: str):
    if not os.path.exists(s):
      raise FileNotFoundError(s)
    return s

  def dir_path(s: str):
    if not os.path.isdir(s):
      raise NotADirectoryError(s)
    return s

  parser.add_argument(
    "input",
    type=file_path,
    help="Path to the nodes enumeration file",
  )
  parser.add_argument(
    "output",
    type=dir_path,
    help="Generated source code destination",
  )

  args = parser.parse_args(namespace=ArgumentsNamespace)

  dest = Path(args.output)

  print(f"Generating AST code from {args.input}...")
  with open(file=args.input, mode="r") as f:
    nodes = parse_nodes(f)

  TARGETS: list[tuple[str, Callable[[AstNodesDict], str]]] = [
    ("ASTNodes.inc", generate_nodes_descriptors),
    ("ASTInspector.inc", generate_macro_inspector),
    ("Dumper.inc", generate_dumper_code),
  ]
  for filepath, transform in TARGETS:
    (dest / filepath).write_text(transform(nodes))

  return 0


if __name__ == "__main__":
  sys.exit(main())
