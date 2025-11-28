from invoke import Context, task

from inv.config import get_build_dir, get_preset
from inv.params import override_params_defaults
from inv.params.build import with_build_params

from . import build

# TODO: unify UT/e2e test targets launch, filters


@task(default=True)
@with_build_params
def test(c: Context):
  c.run(f"ctest --output-on-failure --test-dir '{str(dir)}'")


@task
@override_params_defaults(sanitizers=True)
@with_build_params
def e2e(
  c: Context,
  overwrite_snapshots: bool = False,
):
  build.build(c, target="vanadium_e2e_core")

  dir = get_build_dir(get_preset("debug-sanitizers"))

  args = [
    '--vanadium_testsuites_dir="test/core/suites"',
    *(["--vanadium_overwrite_snapshots"] if overwrite_snapshots else []),
  ]

  c.run(f"{dir}/test/core/runner/vanadium_e2e_core {' '.join(args)}")
