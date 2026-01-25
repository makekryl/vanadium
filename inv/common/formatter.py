import subprocess
from concurrent.futures import ThreadPoolExecutor
from typing import Callable

FormatterCommandBuilder = Callable[[str, bool], list[str]]


def create_parallel_formatter(build_command: FormatterCommandBuilder):
  def run_formatter(filepath: str, dryrun: bool) -> bool:
    result = subprocess.run(
      build_command(filepath, dryrun),
      stdout=subprocess.DEVNULL,
      stderr=subprocess.DEVNULL,
      check=False,
    )
    return result.returncode == 0

  def _run_formatter_in_parallel(
    files: list[str],
    dryrun: bool = True,
    workers: int = 4,
  ):
    baddies = []

    with ThreadPoolExecutor(max_workers=workers) as executor:
      results = executor.map(lambda f: (f, run_formatter(f, dryrun)), files)
      for file, passed in results:
        if not passed:
          baddies.append(file)

    return baddies

  return _run_formatter_in_parallel
