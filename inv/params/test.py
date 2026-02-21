from dataclasses import dataclass
from typing import cast

from invoke import Context

from inv.params import TaskCallable, inject_task_params


@dataclass(slots=True)
class TestOptions:
  jobs: int | None = None
  filter: str | None = None
  exclude: str | None = None
  skip_build: bool = False
  no_run: bool = False
  ctest_args: str | None = None


def test_opts(c: Context):
  return cast(TestOptions, c.config.vanadium["test"])


def with_test_params(tfunc: TaskCallable) -> TaskCallable:
  return inject_task_params(tfunc, "test", TestOptions)
