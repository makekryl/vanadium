from invoke import Context, task

from inv.config import (
  CMAKE_PRESETS,
  TOOLCHAINS,
  get_build_dir,
  get_preset,
)
from inv.params.build import with_build_params


def _get_cmake_params(c: Context):
  def opt(name: str, cond: bool):
    return (name,) if cond else ()

  preset = get_preset(
    config="-".join(
      (
        "release" if c.config.vanadium.build.release else "debug",
        *opt("sanitizers", c.config.vanadium.build.sanitizers),
        *opt("static", c.config.vanadium.build.static),
      )
    ),
    toolchain=c.config.vanadium.build.toolchain,
  )
  build_dir = get_build_dir(preset)
  return preset, build_dir


@task
@with_build_params
def configure(
  c: Context,
):
  preset, build_dir = _get_cmake_params(c)
  c.run(
    f"cmake -DCMAKE_GENERATOR=Ninja --preset '{preset}' -B '{build_dir}'",
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
  _, build_dir = _get_cmake_params(c)

  if c.config.vanadium.build.reconfigure or not build_dir.exists():
    configure(c)

  args = []
  if target:
    args.append(f"--target {target}")
  if c.config.vanadium.build.jobs:
    args.append(f"-j {c.config.vanadium.build.jobs}")

  c.run(f"cmake --build '{build_dir}' {' '.join(args)}")

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
