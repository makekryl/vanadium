# Vanadium

Vanadium is a modern free & open-source TTCN-3 toolset built around a _very_ fast compiler frontend written from scratch, with performance kept in mind.

It provides:

- A feature-complete **language server** for VS Code and other editors, supporting auto-import, ASN.1 and more
- An easy-to-integrate **static code analyzer** that identifies and reports problematic code patterns
- An opinionated **formatter** to keep code stylish and avoid code style discussions and related merge conflicts\*

Currently, Vanadium does not _replace_ existing solutions but instead _complements_ them. There is a rough concept for a compiler and a runtime, which may be worked on after the compiler frontend - currently under active development - has stabilized.

> \* - there's a working prototype of the formatter that needs some work to be rebuild according to the current state of the project, it will be published soon

## Getting started

This section is under construction.

### Installing

Binary distribution will be set up shortly. For now, Vanadium must be built manually; please refer to the relevant section below.

### Declaring a project

To allow Vanadium to understand your project structure, you must declare a project manifest.

Create a file named `.vanadiumrc.toml` in the root of your project.

Example:

```toml
root = true

[project]
name = "my-project"
subprojects = [
  "subproject-1",
  "subproject-2"
]

[external]
external-project-a = { path = "deps/a" }
external-project-b = { path = "deps/b", references = [ "external-project-a" ] }
```

Next, create a .vanadiumrc.toml file inside both `./subproject-1` and `./subproject-2`:

```toml
root = false

[project]
name = "subproject-1"
references = [
  "subproject-2",
  "external-project-b"
]
```

The difference between a "regular" projects and external projects is that external projects do not have to be a Vanadium projects, they may be just a directories with a bunch of TTCN-3 files.

## Comparison to other projects

Unfortunately, the TTCN-3 ecosystem is quite underserved, and there are only a few open-source tools available. As of January 2026, these include:

- [Eclipse Titan](https://gitlab.eclipse.org/eclipse/titan/titan.core) - the only open-source compiler and the de facto standard
- [ntt](https://github.com/nokia/ntt) - editor language support with some basic smart features powered by a fast language server, but limited by the lack of a real semantic & type analyzer
- [Titan Language Server](https://gitlab.eclipse.org/eclipse/titan/titan.language-server) - editor language support, but with performance limitations that make it impractical for larger codebases

Initially, the goal of the Vanadium project was to provide an efficient and feature-complete language server that makes developers’ lives easier, while keeping memory usage low. Below is a performance comparison of language servers, measured on a large project (~600 files, ~250K lines of code) using 4 threads:

|                    | vanadium | ntt         | Titan LS      |
| ------------------ | -------- | ----------- | ------------- |
| Startup time       | 0.45s    | 1.1s        | 73s           |
| Memory usage       | 90 MB    | 350 MB      | 3 GB          |
| ASN.1 support      | +        | -           | partial       |
| Autocompletion     | +        | weak        | weak/too slow |
| Inlay Hints        | +        | partial     | partial       |
| Diagnostics        | +        | syntax only | +             |
| Semantic Highlight | +        | weak        | +             |
| Signature Help     | +        | -           | unstable      |
| Go to Definition   | +        | weak        | unstable      |
| Go to References   | WIP      | weak        | unstable      |
| Formatter          | +        | -           | -             |
| Written in         | C++      | Go          | Java          |

## Contributing

Vanadium is a non-commercial project licensed under the BSD 3-Clause License, and we are happy to accept contributions of all kinds.

### Quick start

Vanadium written in modern C++23, with CMake used as the build system. Python is used as auxilary language for development purposes, [pyinvoke](https://github.com/pyinvoke/invoke) is used for project tasks orchestration and [uv](https://github.com/astral-sh/uv) for Python package and environment management.

Following is required to build the project:
- `cmake`
- `clang >= 19` or `gcc >= 14`
- `python >= 3.12` for AST stuff generation
- `flex` and `bison` to generate ASN.1 parser

Optionally, you may need:
- `ninja` as a faster CMake build backend
- `gdb` and `valgrind` for development
- `clang-format >= 19`

There is a [devcontainer](https://containers.dev/) in the repository that contains all you might need.

> If you use Visual Studio Code too, devcontainers work there out of box, and there are also list of suggested extensions and opinionated workspace configuration template.

Setup a virtual environment and install the dependencies using `uv`:

```sh
$ uv sync
$ source .venv/bin/activate
```

And you will be able to invoke tasks using `inv`. You can check that it is available by executing the following command to list all available tasks:

```sh
(venv) $ inv --list
```

#### Building & launching tools

To build command-line tools like linter or formatter, you may use the following command (`vanadium_tidy` is being used as example):

```sh
$ inv build --target vanadium_tidy --sanitizers --reconfigure
```

Launch:

```sh
$ ./out/build/clang-debug-sanitizers/bin/tidy/vanadium_tidy
```

About the `inv build` flags:
- `--reconfigure` runs CMake reconfiguration before build, it may be skipped if you don't need it
- `--sanitizers` appends `-sanitizers` suffix to the active CMake preset, selecting the corresponding one (see `CMakePresets.json`)
- `--toolchain <clang|gcc>` - selects the active toolchain

#### Building & launching the language server

Vanadium is organized as a monorepo in this matter, i.e. both the language server and the Visual Studio Code extension are being developed here.

```sh
$ inv build --target vanadiumd --sanitizers --reconfigure
$ inv extension.symlink-lsp --sanitizers
```

Now you can launch the extension.

##### Debugging language server

If the language server crashes right after launch, debugging it may be not the most joyful experience out of here. Inside the playground workspace, create `.vscode/settings.json` and add

```json
"vanadiumd.arguments": [
    "--wait-dbg"
]
```

You will have time to attach debugger to the process until it crashes.

#### Running tests

CMake's CTest is used as the test runner and [gtest](https://google.github.io/googletest/) is used as the test framework.

There's two kind of tests - unit tests and end-to-end (e2e) tests. Unit tests are placed within corresponding targets' directories and end-to-end tests are placed in `test/`.

- To run unit tests:
```sh
$ inv test.unit
```

- To run end-to-end tests:
```sh
$ inv test.e2e
```

To filter our exclude specific tests by regular expressions, flags `-f` and `-e` can be used, i.e..

### Design

The core of Vanadium is its compiler frontend, which is designed specifically with language server development in mind. The AST has a very low memory footprint, and the compiler frontend supports incremental analysis while being highly parallel.

Below are some core design principles intended to keep Vanadium fast:

- If dynamic memory allocation is unavoidable, memory should be allocated in bulk and reused as much as possible
- Lambdas are useful: prefer consuming results via callbacks rather than returning dynamically allocated collections, especially when not all results are needed
- Caching is not always beneficial, especially when it increases memory usage
- Inline small functions by placing them in headers, but keep larger ones in source files to avoid slowing down build times
- Parameterize functions at hot paths at compile time using `if constexpr` and pass configuration via compile-time structs injected as template arguments

## Acknowledgements

It would have been impossible to build this project without these wonderful open-source projects:

- **[nokia/ntt](https://github.com/nokia/ntt)** - _ntt_ provided a solid foundation with its performant, handwritten recursive-descent TTCN-3 parser, which was rewritten in C++ and further optimized

- **[vlm/asn1c](https://github.com/vlm/asn1c)** - adding ASN.1 support, with its complex syntax and mind-bending constructs, would have been unbearable without the _asn1c_ parser and the ideas borrowed from its semantic analyzer

- **[stephenberry/glaze](https://github.com/stephenberry/glaze)** - _glaze_ is an efficient, modern C++ JSON and reflection library that made building the language server straightforward and painless

- & more: **[fmtlib/fmt](https://github.com/fmtlib/fmt)**, **[Neargye/magic_enum](https://github.com/Neargye/magic_enum)**, **[p-ranav/argparse](https://github.com/p-ranav/argparse)**, **[getml/reflect-cpp](https://github.com/getml/reflect-cpp)**, **[uxlfoundation/oneTBB](https://github.com/uxlfoundation/oneTBB)**
