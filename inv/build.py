from invoke import Context, task

from ._common import (
  CMAKE_PRESETS,
  DEFAULT_TOOLCHAIN,
  TOOLCHAINS,
  get_build_dir,
  get_preset,
)


@task
def configure(
  c: Context,
  config: str,
  toolchain: str = DEFAULT_TOOLCHAIN,
):
  preset = get_preset(config, toolchain)
  build_dir = get_build_dir(preset)
  c.run(
    f"cmake --preset '{preset}' -B '{build_dir}'",
    env={
      "CMAKE_COLOR_DIAGNOSTICS": "ON",
    },
  )


@task(default=True)
def build(
  c: Context,
  config: str,
  toolchain: str = DEFAULT_TOOLCHAIN,
  target: str | None = None,
  jobs: int | None = None,
  reconfigure: bool = False,
):
  preset = get_preset(config, toolchain)
  build_dir = get_build_dir(preset)

  if reconfigure or not build_dir.exists():
    configure(c, config, toolchain)
    c.run(
      f"cmake -DCMAKE_GENERATOR=Ninja --preset '{preset}' -B '{build_dir}'",
      env={
        "CMAKE_COLOR_DIAGNOSTICS": "ON",
      },
    )

  build_args = []
  #
  if target:
    build_args.append(f"--target {target}")
  if jobs:
    build_args.append(f"-j {jobs}")
  #
  c.run(f"cmake --build '{build_dir}' {' '.join(build_args)}")


@task
def list_configs(c: Context):
  configs: dict[str, set[str]] = {}
  for preset in CMAKE_PRESETS:
    for toolchain in TOOLCHAINS:
      prefix = f"{toolchain}-"
      if preset.startswith(prefix):
        config = preset.removeprefix(prefix)
        if config not in configs:
          configs[config] = set()
        configs[config].add(toolchain)
        break
    else:
      configs[preset] = set()

  print("Available configs:")
  for config, toolchains in configs.items():
    print(f" - {config} ({', '.join(toolchains)})")
