from dataclasses import dataclass
from typing import cast

from invoke import Context

from inv.config import DEFAULT_TOOLCHAIN
from inv.params import TaskCallable, inject_task_params


@dataclass(slots=True)
class BuildOptions:
  toolchain: str = DEFAULT_TOOLCHAIN

  sanitizers: bool = False
  coverage: bool = False

  release: bool = False
  static: bool = False

  # cmake
  reconfigure: bool = False
  cmake_jobs: int | None = None


def build_opts(c: Context):
  return cast(BuildOptions, c.config.vanadium["build"])


def with_build_params(tfunc: TaskCallable) -> TaskCallable:
  return inject_task_params(tfunc, "build", BuildOptions)
