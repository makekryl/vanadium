from typing import Literal

from invoke import Context, task


@task
def test(c: Context, config: Literal["Debug", "Release"] = "Debug"):
  c.run(f"ctest --output-on-failure --test-dir build/{config}", pty=True)


@task
def check_style(c: Context, fix: bool = False):
  STYLE_CHECKERS: list[tuple[str, str]] = [
    # (check, fix)
    ("ruff check --select I", "ruff check --select I --fix"),
    ("ruff format --check", "ruff format"),
    ("npm run format:check", "npm run format"),
  ]
  for cmd in [checker[1 if fix else 0] for checker in STYLE_CHECKERS]:
    c.run(cmd)
