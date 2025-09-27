from invoke import Context, task

from ._common import CMAKE_PRESETS, DEFAULT_TOOLCHAIN, get_build_dir, get_preset


@task(default=True)
def build(
  c: Context,
  config: str,
  toolchain: str = DEFAULT_TOOLCHAIN,
  target: str | None = None,
  jobs: int | None = None,
  reconfigure: bool = True,
):
  preset = get_preset(config, toolchain)
  build_dir = get_build_dir(preset)

  if reconfigure:
    c.run(
      f"cmake -DCMAKE_GENERATOR=Ninja --preset '{preset}' -B '{build_dir}'",
      env={
        "CMAKE_COLOR_DIAGNOSTICS": "ON",
      },
    )

  build_args = []
  if target:
    build_args.append(f"--target {target}")
  if jobs:
    build_args.append(f"-j {jobs}")
  c.run(f"cmake --build '{build_dir}' {' '.join(build_args)}")


@task
def list_configs(c: Context):
  print("\n".join(CMAKE_PRESETS))
