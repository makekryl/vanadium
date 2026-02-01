import sys
import time
from dataclasses import dataclass
from typing import Callable, Iterable

from invoke import Context, task

from inv.common import git
from inv.common.colors import colors
from inv.common.formatter import create_parallel_formatter
from inv.common.utils import get_elapsed_time_msg


@dataclass
class Formatter:
  name: str
  alias: str
  run: Callable  # decltype(inv.common.formatter:create_parallel_formatter(...))
  file_extensions: Iterable[str]


_FORMATTERS: list[Formatter] = [
  Formatter(
    name="clang-format",
    alias="cpp",
    run=create_parallel_formatter(
      lambda filepath, dryrun: [
        "clang-format",
        *(("--dry-run", "--Werror") if dryrun else ("-i",)),
        filepath,
      ]
    ),
    file_extensions=("c", "cc", "cpp", "h", "hh", "hpp"),
  ),
  Formatter(
    name="ruff format",
    alias="py",
    run=create_parallel_formatter(
      lambda filepath, dryrun: [
        "uvx",
        "ruff",
        "format",
        *(("--check",) if dryrun else ()),
        filepath,
      ]
    ),
    file_extensions=("py",),
  ),
  Formatter(
    name="ruff check --select I",
    alias="py",
    run=create_parallel_formatter(
      lambda filepath, dryrun: [
        "uvx",
        "ruff",
        "check",
        "--select",
        "I",
        *(() if dryrun else ("--fix",)),
        filepath,
      ]
    ),
    file_extensions=("py",),
  ),
  Formatter(  # TODO: batching for prettier, too slow startup
    name="prettier",
    alias="js",
    run=create_parallel_formatter(
      lambda filepath, dryrun: [
        "npx",
        "prettier",
        "--check",
        *(() if dryrun else ("--write",)),
        filepath,
      ]
    ),
    file_extensions=("js", "mjs", "cjs", "jsx", "ts", "tsx"),
  ),
]

_IGNORE_FILEPATHS_PREFIXES: dict[str, Iterable[str]] = {
  "cpp": ("src/asn1/asn1c_libasn1parser/",),
}


def _run_formatter(formatter: Formatter, dryrun: bool, workers: int) -> int:
  files = git.list_files(formatter.file_extensions)

  total_files_count = len(files)
  if formatter.alias in _IGNORE_FILEPATHS_PREFIXES:
    files = [
      f
      for f in files
      if not any(f.startswith(p) for p in _IGNORE_FILEPATHS_PREFIXES[formatter.alias])
    ]

  print_prefix = f"{colors.CYAN}{colors.BOLD}{formatter.alias}:{colors.END}"
  print(
    print_prefix,
    f"running {colors.CYAN}{formatter.name}{colors.END} "
    f"over {len(files)} out of {total_files_count} files in {workers} threads",
  )

  ts_start = time.time()
  baddies = formatter.run(files, dryrun, workers)
  elapsed = time.time() - ts_start

  if not baddies:
    print("", print_prefix, "all files are formatted", get_elapsed_time_msg(elapsed))
    return 0

  print(
    "",
    print_prefix,
    f"{len(baddies)} files are not formatted: {', '.join(f"'{colors.PURPLE}{b}{colors.END}'" for b in baddies)}",
    get_elapsed_time_msg(elapsed),
  )

  return len(baddies)


@task
def format(c: Context, fix: bool = False, workers: int = 4):
  baddies = 0

  ts_start = time.time()
  for formatter in _FORMATTERS:
    baddies += _run_formatter(
      formatter=formatter,
      dryrun=not fix,
      workers=workers,
    )
  elapsed = time.time() - ts_start

  if baddies > 0:
    print(
      f"{colors.RED}{colors.BOLD}✕ {baddies} files are not formatted{colors.END}",
      get_elapsed_time_msg(elapsed),
    )
    sys.exit(1)
    return

  print(
    f"{colors.GREEN}{colors.BOLD}✓ All files are formatted{colors.END}",
    get_elapsed_time_msg(elapsed),
  )
