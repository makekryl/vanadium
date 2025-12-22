import errno
import os
from pathlib import Path

from invoke import Context, task

from inv.config import get_build_dir
from inv.params.build import with_build_options_params, with_build_params

from . import build


@task
@with_build_options_params
def symlink_lsp(c: Context):
  dst = Path("extension/bin/vanadiumd")
  _, build_dir = build._get_cmake_params(c)

  def _create_symlink():
    return os.symlink((build_dir / "bin/lsp/vanadiumd").absolute(), dst)

  try:
    _create_symlink()
  except OSError as err:
    if err.errno != errno.EEXIST:
      raise err
    dst.unlink()
    _create_symlink()


@task
@with_build_params
def package(c: Context):
  build.build(c, target="vanadiumd")
  _, build_dir = build._get_cmake_params(c)

  c.run("rm -f 'extension/bin/vanadiumd'")
  c.run(f"cp '{build_dir / '/bin/lsp/vanadiumd'}' 'extension/bin'")

  with c.cd("extension"):
    c.run("npx vsce package")
