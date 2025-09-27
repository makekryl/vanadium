from invoke import Collection, Context, Task, UnexpectedExit, task
from invoke.runners import Local as LocalRunner
from invoke.runners import Result as RunResult

from inv import build, check, extension, test


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
          "\033[1;31m ✕ Command '\033[22m{cmd}\033[1m' exited with code {code}\033[0m".format(
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
      "echo_format": "\033[1;37m ⟶ {command}\033[0m",
      "echo": True,
      "pty": True,
    },
    "runners": {
      "local": AlertingLocalRunner,
    },
  },
)
