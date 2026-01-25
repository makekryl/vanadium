import subprocess
from typing import Iterable


def list_files(extensions: Iterable[str]) -> list[str]:
  return subprocess.run(
    ["git", "ls-files"] + [f"*.{ext}" for ext in extensions],
    capture_output=True,
    text=True,
    check=True,
  ).stdout.splitlines()
