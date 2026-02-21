from invoke import Context, task

from inv.params import override_params_defaults
from inv.params.build import with_build_params, with_cmake_params
from inv.params.test import test_opts, with_test_params

from . import build


@task(default=True)
@override_params_defaults(sanitizers=True)
@with_build_params
@with_cmake_params
@with_test_params
def test(c: Context, label: str):
  if not test_opts(c).skip_build:
    build_dir = build.build(c, target=f"build_{label}_tests")
  else:
    build_dir = build.get_build_dir(c)

  if test_opts(c).no_run:
    return

  args = []
  if test_opts(c).filter:
    args.append(f"-R '{test_opts(c).filter}'")
  if test_opts(c).exclude:
    args.append(f"-E '{test_opts(c).exclude}'")
  if test_opts(c).jobs:
    args.append(f"-j {test_opts(c).jobs}")
  if test_opts(c).ctest_args:
    args.append(test_opts(c).ctest_args)

  c.run(
    f"ctest -L '{label}' --output-on-failure --test-dir '{str(build_dir)}' {' '.join(args)}"
  )


@task
@override_params_defaults(sanitizers=True)
@with_build_params
@with_cmake_params
@with_test_params
def unit(c: Context):
  test(c, label="unit")


@task
@override_params_defaults(sanitizers=True)
@with_build_params
@with_cmake_params
@with_test_params
def e2e(
  c: Context,
  overwrite_snapshots: bool = False,
):
  test(c, label="e2e")
