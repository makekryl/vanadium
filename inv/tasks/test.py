from invoke import Context, task

from inv.params import override_params_defaults
from inv.params.build import with_build_params
from inv.params.test import with_test_params

from . import build


@task(default=True)
@override_params_defaults(sanitizers=True)
@with_build_params
@with_test_params
def test(c: Context, label: str):
  build_dir = build.build(c, target=f"build_{label}_tests")

  if c.config.vanadium.test.only_build:
    return

  args = []
  if c.config.vanadium.test.filter:
    args.append(f"-R '{c.config.vanadium.test.filter}'")
  if c.config.vanadium.test.jobs:
    args.append(f"-j {c.config.vanadium.test.jobs}")
  if c.config.vanadium.test.ctest_args:
    args.append(c.config.vanadium.test.ctest_args)

  c.run(
    f"ctest -L '{label}' --output-on-failure --test-dir '{str(build_dir)}' {' '.join(args)}"
  )


@task
@override_params_defaults(sanitizers=True)
@with_build_params
@with_test_params
def unit(c: Context):
  test(c, label="unit")


@task
@override_params_defaults(sanitizers=True)
@with_build_params
@with_test_params
def e2e(
  c: Context,
  overwrite_snapshots: bool = False,
):
  test(c, label="e2e")
