from typing import cast

from invoke import Config, Context

from inv.config import DEFAULT_TOOLCHAIN
from inv.params import InjectableParam, TaskCallable, inject_task_params


def with_build_options_params(tfunc: TaskCallable) -> TaskCallable:
  def accept(
    c: Context,
    toolchain: str,
    sanitizers: bool,
    release: bool,
    static: bool,
  ):
    if "build" not in c.config.vanadium:
      c.config.vanadium.build = {}  # c.config is an invoke.DataProxy
    cfg = cast(Config, c.config.vanadium.build)

    cfg.toolchain = toolchain
    cfg.sanitizers = sanitizers
    cfg.release = release
    cfg.static = static

  return inject_task_params(
    tfunc,
    [
      InjectableParam(
        "toolchain",
        DEFAULT_TOOLCHAIN,
      ),
      InjectableParam(
        "sanitizers",
        False,
      ),
      InjectableParam(
        "release",
        False,
      ),
      InjectableParam(
        "static",
        False,
      ),
    ],
    accept,
  )


def with_cmake_params(tfunc: TaskCallable) -> TaskCallable:
  def accept(
    c: Context,
    reconfigure: bool,
    cmake_jobs: int | None,
  ):
    if "build" not in c.config.vanadium:
      c.config.vanadium.build = {}  # c.config is an invoke.DataProxy
    cfg = cast(Config, c.config.vanadium.build)

    cfg.reconfigure = reconfigure
    cfg.jobs = cmake_jobs

  return inject_task_params(
    tfunc,
    [
      InjectableParam(
        "reconfigure",
        False,
      ),
      InjectableParam(
        "cmake_jobs",
        None,
      ),
    ],
    accept,
  )


def with_build_params(tfunc: TaskCallable) -> TaskCallable:
  return with_build_options_params(with_cmake_params(tfunc))
