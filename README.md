# Vanadium


## Contributing

### Quick start

Vanadium is using [pyinvoke](https://github.com/pyinvoke/invoke) for project tasks orchestration
and [uv](https://github.com/astral-sh/uv) for Python package and environment management.

Setup a virtual environment and install the dependencies using `uv`:

```sh
$ uv sync
$ source .venv/bin/activate
```

And you will be able to invoke tasks using `inv`. You can check that it is available by executing the following command to list all available tasks:

```sh
(venv) $ inv --list
```
