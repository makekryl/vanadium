from dataclasses import dataclass
from typing import cast

from invoke import Context

from inv.config import DEFAULT_TOOLCHAIN
from inv.params import TaskCallable, inject_task_params


@dataclass(slots=True)
class BuildOptions:
  toolchain: str = DEFAULT_TOOLCHAIN
  sanitizers: bool = False
  release: bool = False
  static: bool = False


def build_opts(c: Context):
  return cast(BuildOptions, c.config.vanadium["build"])


def with_build_params(tfunc: TaskCallable) -> TaskCallable:
  return inject_task_params(tfunc, "build", BuildOptions)


@dataclass(slots=True)
class CMakeOptions:
  reconfigure: bool = False
  cmake_jobs: int | None = None


def cmake_opts(c: Context):
  return cast(CMakeOptions, c.config.vanadium["cmake"])


def with_cmake_params(tfunc: TaskCallable) -> TaskCallable:
  return inject_task_params(tfunc, "cmake", CMakeOptions)
