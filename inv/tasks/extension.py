from pathlib import Path

from invoke import Context, task

from inv.common.fsutils import force_create_symlink
from inv.config import OUTPUT_DIR
from inv.params.build import with_build_options_params, with_build_params

from . import build


@task
@with_build_options_params
def symlink_lsp(c: Context):
  dst = Path("extension/bin/vanadiumd")

  _, build_dir = build._get_cmake_params(c)
  lsp_bin_path = build_dir / "bin/lsp/vanadiumd"

  print(f"Creating symlink to '{lsp_bin_path}'")
  force_create_symlink(lsp_bin_path, dst)


@task
@with_build_params
def package(c: Context):
  build_dir = build.build(c, target="vanadiumd")

  c.run("rm -f 'extension/bin/vanadiumd'")
  c.run(f"cp '{build_dir / 'bin/lsp/vanadiumd'}' 'extension/bin'")

  vsce_out_dir = OUTPUT_DIR / "vsce"
  vsce_out_dir.mkdir(exist_ok=True)

  with c.cd("extension"):
    args = [
      f"--out '../{vsce_out_dir}'",
    ]
    c.run(f"npx vsce package {' '.join(args)}")
