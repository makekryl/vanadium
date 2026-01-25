import subprocess
from concurrent.futures import ThreadPoolExecutor


def run_clang_format(filepath: str, dryrun: bool) -> bool:
  # Check formatting without changing the file
  result = subprocess.run(
    [
      "clang-format",
      *(
        (
          "--dry-run",
          "--Werror",
        )
        if dryrun
        else ("-i",)
      ),
      filepath,
    ],
    capture_output=True,
  )
  return result.returncode == 0


def run_clang_format_parallel(
  files: list[str],
  dryrun: bool = True,
  workers: int = 4,
) -> list[str]:
  baddies = []

  with ThreadPoolExecutor(max_workers=workers) as executor:
    results = executor.map(lambda f: (f, run_clang_format(f, dryrun)), files)
    for file, passed in results:
      if not passed:
        baddies.append(file)

  return baddies
