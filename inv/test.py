from invoke import Context, task

from ._common import DEFAULT_TOOLCHAIN, get_build_dir, get_preset


@task(default=True)
def test(c: Context, config: str, toolchain: str = DEFAULT_TOOLCHAIN):
  dir = get_build_dir(get_preset(config, toolchain))
  c.run(f"ctest --output-on-failure --test-dir '{str(dir)}'")
