import inspect
from dataclasses import dataclass, fields
from functools import wraps
from inspect import Parameter, Signature, _empty, signature
from typing import Any, Callable, TypeVar

from invoke import Context


@dataclass(frozen=True, slots=True)
class InjectableParam:
  name: str
  default: Any | None | type[_empty] = _empty
  description: str | None = None


_injected_params = set[str]()


TaskCallable = TypeVar("TaskCallable", bound=Callable)


def inject_task_params(
  tfunc: TaskCallable,
  key: str,
  ParamsDescriptor: type,
) -> TaskCallable:
  params_group = inspect.stack()[1].function

  params = [
    InjectableParam(
      name=f.name,
      default=f.default,
    )
    for f in fields(ParamsDescriptor)
  ]

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
      # c.config is DataProxy so it should be overriden like below
      # TODO: write a very tiny pyinvoke replacement
      storage = (
        c.config.vanadium[key] if key in c.config.vanadium else ParamsDescriptor()
      )
      for kw, val in injected_kwargs.items():
        setattr(storage, kw, val)
      c.config.vanadium[key] = storage
      #
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
