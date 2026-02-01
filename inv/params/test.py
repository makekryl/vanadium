from typing import cast

from invoke import Config, Context

from inv.params import InjectableParam, TaskCallable, inject_task_params


def with_test_params(tfunc: TaskCallable) -> TaskCallable:
  def accept(
    c: Context,
    jobs: int | None = None,
    filter: str | None = None,
    exclude: str | None = None,
    skip_build: bool = False,
    no_run: bool = False,
    ctest_args: str | None = None,
  ) -> None:
    if "test" not in c.config.vanadium:
      c.config.vanadium.test = {}  # c.config is an invoke.DataProxy
    cfg = cast(Config, c.config.vanadium.test)

    cfg.jobs = jobs
    cfg.filter = filter
    cfg.exclude = exclude
    cfg.skip_build = skip_build
    cfg.no_run = no_run
    cfg.ctest_args = ctest_args

  return inject_task_params(
    tfunc,
    [
      InjectableParam(
        "jobs",
        None,
      ),
      InjectableParam(
        "filter",
        None,
      ),
      InjectableParam(
        "exclude",
        None,
      ),
      InjectableParam(
        "skip_build",
        False,
      ),
      InjectableParam(
        "no_run",
        False,
      ),
      InjectableParam(
        "ctest_args",
        None,
      ),
    ],
    accept,
  )
