import json
from pathlib import Path

DEFAULT_TOOLCHAIN = "gcc"
CMAKE_PRESETS = [
  preset["name"]
  for preset in json.loads(Path("CMakePresets.json").read_text())["configurePresets"]
  if "hidden" not in preset or not preset["hidden"]
]


def get_preset(config: str, toolchain: str = DEFAULT_TOOLCHAIN):
  preset = f"{toolchain}-{config}"
  assert preset in CMAKE_PRESETS
  return preset


def get_build_dir(preset: str) -> Path:
  return Path("out") / "build" / preset
