def _sanitize(line: str) -> str:
  return line.replace("/*", "/\\*")


def lines_to_multiline_comment(lines: list[str]) -> list[str]:
  return [
    "/**",
    *[f" * {_sanitize(line)}" for line in lines],
    " */",
  ]


def lines_to_comment(lines: list[str]) -> list[str]:
  return [f"// {_sanitize(line)}" for line in lines]


def as_enum_v(name: str):
  return f"k{name.capitalize()}"
