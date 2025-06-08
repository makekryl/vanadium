import subprocess
from pathlib import Path

if __name__ == "__main__":
  subprocess.run(
    [
      "uv",
      "run",
      "python",
      "-m",
      "generator",
      "--plugin",
      "cpp",
      "--output-dir",
      ".",
    ],
    cwd=Path(__file__).parent,
  )
