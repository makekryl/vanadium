from invoke import Collection, Context, Task, UnexpectedExit, task
from invoke.runners import Local as LocalRunner
from invoke.runners import Result as RunResult

from inv.common.colors import colors
from inv.tasks import build, check, extension, test


@task
def regenerate_lsprotocol(c: Context):
  c.run("python3 ./lib/lsprotocol/run_generator.py")


class AlertingLocalRunner(LocalRunner):
  def run(self, command: str, **kwargs):
    try:
      return super().run(command, **kwargs)
    except UnexpectedExit as err:
      if not kwargs.get("hide"):
        result: RunResult = err.result
        print(
          f"{colors.RED}{colors.BOLD} ✕ Command '{colors.NO_INTENSITY}{{cmd}}{colors.BOLD}' exited with code {{code}}{colors.END}".format(
            cmd=result.command,
            code=result.return_code,
          )
        )
      raise err


ns = Collection(*(item for _, item in locals().items() if isinstance(item, Task)))
#
ns.add_collection(build)
ns.add_collection(check)
ns.add_collection(extension)
ns.add_collection(test)
#
ns.configure(
  {
    "run": {
      "echo_format": f"{colors.LIGHT_GRAY}{colors.BOLD} ⟶ {{command}}{colors.END}",
      "echo": True,
      "pty": True,
    },
    "runners": {
      "local": AlertingLocalRunner,
    },
    "vanadium": {},  # vanadium tasks specific config
  },
)
