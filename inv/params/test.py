from typing import Callable, cast

from invoke import Config, Context

from inv.params import InjectableParam, inject_task_params


def with_test_params(tfunc: Callable):
  def accept(
    c: Context,
    jobs: int | None = None,
    filter: str | None = None,
  ) -> None:
    if "test" not in c.config.vanadium:
      c.config.vanadium.test = {}  # c.config is an invoke.DataProxy
    cfg = cast(Config, c.config.vanadium.test)

    cfg.jobs = jobs
    cfg.filter = filter

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
    ],
    accept,
  )
