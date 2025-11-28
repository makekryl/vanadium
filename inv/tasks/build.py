from invoke import Context, task

from inv.config import (
  CMAKE_PRESETS,
  TOOLCHAINS,
  get_build_dir,
  get_preset,
)
from inv.params.build import with_build_params


def _get_config_name(c: Context):
  return "-".join(
    (
      "release" if c.config.vanadium.build.release else "debug",
      *(("sanitizers",) if c.config.vanadium.build.sanitizers else ()),
    )
  )


@task
@with_build_params
def configure(
  c: Context,
):
  preset = get_preset(
    config=_get_config_name(c),
    toolchain=c.config.vanadium.build.toolchain,
  )
  build_dir = get_build_dir(preset)
  c.run(
    f"cmake --preset '{preset}' -B '{build_dir}'",
    env={
      "CMAKE_COLOR_DIAGNOSTICS": "ON",
    },
  )


@task(default=True)
@with_build_params
def build(
  c: Context,
  target: str | None = None,
):
  preset = get_preset(
    config=_get_config_name(c),
    toolchain=c.config.vanadium.build.toolchain,
  )
  build_dir = get_build_dir(preset)

  if c.config.vanadium.build.reconfigure or not build_dir.exists():
    configure(c)
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
  if c.config.vanadium.build.cmake_jobs:
    build_args.append(f"-j {c.config.vanadium.build.cmake_jobs}")
  #
  c.run(f"cmake --build '{build_dir}' {' '.join(build_args)}")

  return build_dir


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
