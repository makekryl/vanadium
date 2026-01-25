from inv.common.colors import colors


def get_elapsed_time_msg(elapsed: float):
  return f"{colors.LIGHT_GRAY}({round(elapsed, 1)} s){colors.END}"
