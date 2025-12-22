import inspect
from dataclasses import dataclass
from functools import wraps
from inspect import Parameter, Signature, _empty, signature
from typing import Callable, ParamSpec, TypeVar

from invoke import Context


@dataclass(frozen=True, slots=True)
class InjectableParam:
  name: str
  default: str | int | bool | None | type[_empty] = _empty
  description: str | None = None


_injected_params = set[str]()


TaskCallable = TypeVar("TaskCallable", bound=Callable)


def inject_task_params(
  tfunc: TaskCallable,
  params: list[InjectableParam],
  accept: Callable[..., None],
) -> TaskCallable:
  params_group = inspect.stack()[1].function

  @wraps(tfunc)
  def wrapper(
    c: Context,
    **kwargs,
  ):
    param_names = {p.name for p in params}
    task_kwargs, injected_kwargs = {}, {}
    for kw, val in kwargs.items():
      (injected_kwargs if kw in param_names else task_kwargs)[kw] = val

    if params_group not in _injected_params:
      accept(c=c, **injected_kwargs)
      _injected_params.add(params_group)

    return tfunc(
      c,
      **task_kwargs,
    )

  wrapper.__signature__ = Signature(  # type: ignore
    list(signature(tfunc).parameters.values())
    + [
      Parameter(
        param.name,
        Parameter.KEYWORD_ONLY,
        default=param.default,
      )
      for param in params
    ]
  )

  help_dict = getattr(wrapper, "help", {})
  help_dict.update(
    {param.name: param.description for param in params if param.description is not None}
  )
  wrapper.help = help_dict  # type: ignore

  return wrapper


def override_params_defaults(**overrides):
  def decorator(func):
    sig = inspect.signature(func)
    params = list(sig.parameters.values())

    for i, param in enumerate(params):
      if param.name in overrides:
        params[i] = param.replace(default=overrides[param.name])

    @wraps(func)
    def wrapper(*args, **kwargs):
      return func(*args, **kwargs)

    wrapper.__signature__ = sig.replace(parameters=params)  # type: ignore
    return wrapper

  return decorator
