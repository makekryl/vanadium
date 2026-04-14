from invoke import Context, task

from inv.config import OUTPUT_DIR
from inv.params import override_params_defaults
from inv.params.build import with_build_params

from . import build


@task
@override_params_defaults(release=True)
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
