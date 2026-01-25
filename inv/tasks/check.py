from typing import Iterable

from invoke import Context, task

from inv.common import git
from inv.common.clang_format import run_clang_format_parallel

_IGNORE_FILE_PREFIXES: dict[str, Iterable[str]] = {
  "cpp": ("src/asn1/asn1c_libasn1parser/",),
}


def _run_clang_format_check(fix: bool):
  files = [
    f
    for f in git.list_files(("c", "cc", "cpp", "h", "hh", "hpp"))
    if not any(f.startswith(p) for p in _IGNORE_FILE_PREFIXES["cpp"])
  ]
  workers = 4

  print(f"Running clang-format over {len(files)} files in {workers} threads...")

  baddies = run_clang_format_parallel(
    files=files,
    workers=workers,
    dryrun=not fix,
  )

  if not baddies:
    print("All files are formatted!")
    return

  print("Some files are not formatted:")
  for baddie in baddies:
    print(" -", baddie)


@task
def style(c: Context, fix: bool = False):
  _run_clang_format_check(fix)

  for checker in (
    # (check, fix)
    ("ruff check --select I", "ruff check --select I --fix"),
    ("ruff format --check", "ruff format"),
    ("npx prettier --check extension/", "npx prettier --write extension/"),
  ):
    c.run(checker[1 if fix else 0], warn=True)
