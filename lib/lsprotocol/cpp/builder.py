from contextlib import contextmanager
from typing import Self

from attr import dataclass


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


@dataclass
class TypeEntry:
  buf: SourceCodeBuilder
  dependencies: list[str] = []


TypesRegistry = dict[str, TypeEntry]
