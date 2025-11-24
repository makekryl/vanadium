from invoke import Context, call, task

from inv import build

from ._common import DEFAULT_TOOLCHAIN, get_build_dir, get_preset

# TODO: unify UT/e2e test targets launch, filters


@task(default=True)
def test(c: Context, config: str, toolchain: str = DEFAULT_TOOLCHAIN):
  dir = get_build_dir(get_preset(config, toolchain))
  c.run(f"ctest --output-on-failure --test-dir '{str(dir)}'")


@task(call(build.build, config="debug-sanitizers", target="vanadium_e2e_core"))
def e2e(
  c: Context,
  overwrite_snapshots: bool = False,
):
  dir = get_build_dir(get_preset("debug-sanitizers"))

  args = [
    '--vanadium_testsuites_dir="test/core/suites"',
    *(["--vanadium_overwrite_snapshots"] if overwrite_snapshots else []),
  ]

  c.run(f"{dir}/test/core/runner/vanadium_e2e_core {' '.join(args)}")
