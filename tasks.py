import errno
import os
from pathlib import Path
from typing import Literal

from invoke import Context, task


@task
def symlink_vanadiumd(c: Context, config: Literal["Debug", "Release"] = "Debug"):
  dst = Path("./extension/bin/vanadiumd")

  def _create_symlink():
    return os.symlink(Path(f"./build/{config}/bin/lsp/vanadiumd").absolute(), dst)

  try:
    _create_symlink()
  except OSError as err:
    if err.errno != errno.EEXIST:
      raise err
    dst.unlink()
    _create_symlink()


@task
def test(c: Context, config: Literal["Debug", "Release"] = "Debug"):
  c.run(f'ctest --output-on-failure --test-dir "./build/{config}"', pty=True)


@task
def check_style(c: Context, fix: bool = False):
  STYLE_CHECKERS: list[tuple[str, str]] = [
    # (check, fix)
    ("ruff check --select I", "ruff check --select I --fix"),
    ("ruff format --check", "ruff format"),
    ("npm run format:check", "npm run format"),
  ]
  for cmd in [checker[1 if fix else 0] for checker in STYLE_CHECKERS]:
    c.run(cmd)


@task
def regenerate_lsprotocol(c: Context):
  c.run("python3 ./lib/lsprotocol/run_generator.py")
