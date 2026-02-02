import sys
from pathlib import Path

from invoke import Context, task

from inv.common.fsutils import force_create_symlink
from inv.config import (
  CMAKE_PRESETS,
  TOOLCHAINS,
  get_preset,
  get_preset_build_dir,
)
from inv.params.build import with_build_params


def _cmake_bool(b: bool):
  return "ON" if b else "OFF"


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
  build_dir = get_preset_build_dir(preset)
  return preset, build_dir


def get_build_dir(c: Context):
  _, build_dir = _get_cmake_params(c)
  return build_dir


@task
@with_build_params
def configure(
  c: Context,
):
  preset, build_dir = _get_cmake_params(c)

  use_compile_commands = True

  c.run(
    f"cmake -DCMAKE_GENERATOR=Ninja --preset '{preset}' -B '{build_dir}'",
    env={
      "CMAKE_EXPORT_COMPILE_COMMANDS": _cmake_bool(use_compile_commands),
      "CMAKE_COLOR_DIAGNOSTICS": _cmake_bool(sys.stdout.isatty()),
    },
  )

  if use_compile_commands:
    compile_commands_filename = "compile_commands.json"
    force_create_symlink(
      build_dir / compile_commands_filename, Path(compile_commands_filename)
    )


@task(default=True)
@with_build_params
def build(
  c: Context,
  target: str | None = None,
):
  build_dir = get_build_dir(c)

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
@with_build_params
def install(
  c: Context,
  dir: str | None = None,
):
  build_dir = get_build_dir(c)

  args = [
    "--component runtime",
  ]
  if dir:
    assert Path(dir).exists(), f"directory '{dir}' does not exist"
    args.append(f"--prefix '{dir}'")

  c.run(f"cmake --install '{build_dir}' {' '.join(args)}")

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
