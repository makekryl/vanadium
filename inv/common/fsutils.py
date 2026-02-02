import errno
import os
from pathlib import Path


def force_create_symlink(src: Path, dst: Path):
  def _create_symlink():
    return os.symlink(src.absolute(), dst)

  try:
    _create_symlink()
  except OSError as err:
    if err.errno != errno.EEXIST:
      raise err
    dst.unlink()
    _create_symlink()
