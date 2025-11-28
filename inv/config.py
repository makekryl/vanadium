import json
from pathlib import Path

TOOLCHAINS = ("gcc", "clang")
CMAKE_PRESETS: list[str] = [
  preset["name"]
  for preset in json.loads(Path("CMakePresets.json").read_text())["configurePresets"]
  if "hidden" not in preset or not preset["hidden"]
]


DEFAULT_TOOLCHAIN: str = TOOLCHAINS[0]


def get_preset(config: str, toolchain: str = DEFAULT_TOOLCHAIN):
  preset = f"{toolchain}-{config}"
  assert preset in CMAKE_PRESETS, f"Preset '{preset}' does not exist"
  return preset


def get_build_dir(preset: str) -> Path:
  return Path("out") / "build" / preset
