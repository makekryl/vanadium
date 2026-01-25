import subprocess
from typing import Iterable


def list_files(extensions: Iterable[str]) -> list[str]:
  base_command = [
    "git",
    "ls-files",
    "--exclude-standard",
  ] + [f"*.{ext}" for ext in extensions]

  def query(args) -> list[str]:
    return subprocess.run(
      args,
      capture_output=True,
      text=True,
      check=True,
    ).stdout.splitlines()

  return list(set(query(base_command)) - set(query(base_command + ["--deleted"])))
