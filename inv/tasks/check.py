from invoke import Context, task


@task
def style(c: Context, fix: bool = False):
  for checker in (
    # (check, fix)
    ("ruff check --select I", "ruff check --select I --fix"),
    ("ruff format --check", "ruff format"),
    ("npm run format:check", "npm run format"),
  ):
    c.run(checker[1 if fix else 0], warn=True)
