import os
import shutil

from invoke import Context, task

from inv.config import OUTPUT_DIR
from inv.params import override_params_defaults
from inv.params.build import build_opts, with_build_params
from inv.params.test import test_opts, with_test_params

from . import build

COVERAGE_DIR = OUTPUT_DIR / "coverage"

TEST_PARAMS_DEFAULTS = {"sanitizers": True}


@task(default=True)
@override_params_defaults(**TEST_PARAMS_DEFAULTS)
@with_build_params
@with_test_params
def test(c: Context, label: str = "all"):
  if not test_opts(c).skip_build:
    build_dir = build.build(c, target=f"build_{label}_tests")
  else:
    build_dir = build.get_build_dir(c)

  if test_opts(c).no_run:
    return

  args = []
  if test_opts(c).filter:
    args.append(f"-R '{test_opts(c).filter}'")
  if test_opts(c).exclude:
    args.append(f"-E '{test_opts(c).exclude}'")
  if test_opts(c).jobs:
    args.append(f"-j {test_opts(c).jobs}")
  if test_opts(c).verbose:
    args.append("-V")
  if test_opts(c).ctest_args:
    args.append(test_opts(c).ctest_args)

  label_opt = f"-L '{label}'" if label != "all" else ""
  c.run(
    f"ctest"
    f" {label_opt}"
    f" --output-on-failure"
    f" --test-dir '{str(build_dir)}'"
    f" {' '.join(args)}"
  )

  if test_opts(c).report_coverage:
    report_coverage(c, jobs=test_opts(c).jobs or 1)


@task
@override_params_defaults(**TEST_PARAMS_DEFAULTS)
@with_build_params
@with_test_params
def unit(c: Context):
  test(c, label="unit")


@task
@override_params_defaults(**TEST_PARAMS_DEFAULTS)
@with_build_params
@with_test_params
def e2e(
  c: Context,
  overwrite_snapshots: bool = False,
):
  test(c, label="e2e")


@task
@override_params_defaults(**TEST_PARAMS_DEFAULTS)
@with_build_params
def report_coverage(c: Context, jobs: int = 1):
  build_dir = build.get_build_dir(c)

  if COVERAGE_DIR.exists():
    old_coverage_dir = COVERAGE_DIR.with_suffix(".old")
    if old_coverage_dir.exists():
      shutil.rmtree(old_coverage_dir)
    os.rename(COVERAGE_DIR, old_coverage_dir)
  COVERAGE_DIR.mkdir()

  excluded_src_dirs = {
    build_dir / "_deps",
  }
  excluded_src_dirs_args = " ".join(f"--exclude '{dir}'" for dir in excluded_src_dirs)

  gcov_exec = "llvm-cov gcov" if build_opts(c).toolchain == "clang" else "gcov"
  c.run(
    f"gcovr"
    f" --root .."
    f" --object-directory '{build_dir}'"
    f" --gcov-executable '{gcov_exec}'"
    f" {excluded_src_dirs_args}"
    f" --print-summary"
    f" --html --html-details -o '{COVERAGE_DIR}/index.html'"
    f" -j {jobs}"
  )
  print(f"Coverage report is available at '{COVERAGE_DIR}'")


@task
def serve_coverage(c: Context):
  if not COVERAGE_DIR.exists():
    print(f"Coverage report not found at '{COVERAGE_DIR}'")
    exit(1)  # TODO(inv): add something like failure(err_msg) that exits
    return

  c.run(f"python3 -m http.server -d '{COVERAGE_DIR}'")
