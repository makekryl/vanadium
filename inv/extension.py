import errno
import os
from pathlib import Path

from invoke import Context, call, task

from . import build
from ._common import get_build_dir


@task
def symlink_lsp(c: Context, config):
  dst = Path("extension/bin/vanadiumd")
  build_dir = get_build_dir(config)

  def _create_symlink():
    return os.symlink((build_dir / "bin/lsp/vanadiumd").absolute(), dst)

  try:
    _create_symlink()
  except OSError as err:
    if err.errno != errno.EEXIST:
      raise err
    dst.unlink()
    _create_symlink()


@task(call(build.build, config="release-static", target="vanadiumd"))
def package(c: Context):
  c.run("rm -f 'extension/bin/vanadiumd'")
  c.run(
    f"cp '{str(get_build_dir('gcc-release-static'))}/bin/lsp/vanadiumd' 'extension/bin'"
  )
  with c.cd("extension"):
    c.run("npx vsce package")
