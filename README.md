# ccmd

[简体中文](docs/README_zh-CN.md)

`ccmd` is a header-only command-line framework for C++11 and later. It adds
commands, nested subcommands, callbacks, and generated help on top of
[`cflag`](https://github.com/locallocal/cflag), which performs option parsing
and value conversion.

## Features

- Header-only `ccmd` API with a CMake interface target
- Commands and arbitrarily nested subcommands
- Long and short options
- `bool`, `int`, `float`, and `string` option values
- Positional arguments and the `--` option terminator
- Command callbacks and generated help
- C++11-compatible public API

## Requirements

- A C++11-compatible compiler
- CMake 3.16 or newer
- The `cflag` Git submodule

Clone the repository and initialize the dependency in one step:

```bash
git clone --recurse-submodules https://github.com/locallocal/ccmd.git
cd ccmd
```

For an existing clone, initialize the dependency with:

```bash
git submodule update --init --recursive
```

`ccmd` itself does not produce a library archive. The exported `ccmd::ccmd`
interface target supplies its headers and links the compiled `cflag`
dependency transitively.

## Quick start

```cpp
#include <ccmd.h>

#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    auto command = std::make_shared<ccmd::c_command>(
        "server",
        "server --port=8080",
        "server [options]",
        "Start the example server.",
        "start a server",
        [](const std::shared_ptr<ccmd::c_command> &active) {
            std::cout << "verbose: " << std::boolalpha
                      << active->bool_var("verbose") << '\n';
            std::cout << "port: " << active->int_var("port") << '\n';
        }
    );

    command->bool_varp("verbose", "v", false, "enable verbose output");
    command->int_varp("port", "p", 8080, "server port");
    command->execute(argc, argv);
}
```

Build and run the repository example:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/ccmd-example --help
```

Create commands with `std::make_shared`. Command callbacks call
`shared_from_this()` internally and therefore require the command to be owned
by a `std::shared_ptr`.

## Subcommands

Register a subcommand with `add_subcommand`:

```cpp
auto root = std::make_shared<ccmd::c_command>(
    "server",
    "server start --port=8080",
    "server <command> [options]",
    "Manage the server.",
    "server management"
);

auto start = std::make_shared<ccmd::c_command>(
    "start",
    "server start --port=8080",
    "server start [options]",
    "Start the server process.",
    "start the server",
    [](const std::shared_ptr<ccmd::c_command> &active) {
        std::cout << "port: " << active->int_var("port") << '\n';
    }
);

start->int_varp("port", "p", 8080, "server port");
root->add_subcommand(start);
root->execute(argc, argv);
```

Subcommands can contain their own subcommands. Each command owns an independent
option set and receives only its own positional arguments.

## Options

Every supported value type provides three operations:

- `*_var(name, default_value, usage)` registers a long option.
- `*_varp(name, short_name, default_value, usage)` registers long and short
  names for the same option.
- `*_var(name)` returns the parsed value.

```cpp
command->bool_varp("verbose", "v", false, "enable verbose logging");
command->int_varp("port", "p", 8080, "server port");
command->float_var("ratio", 0.5F, "sampling ratio");
command->string_varp("config", "c", "server.conf", "configuration file");

bool verbose = command->bool_var("verbose");
int port = command->int_var("p"); // Long and short names both work.
float ratio = command->float_var("ratio");
const std::string &config = command->string_var("config");
```

Both string literals and `const std::string` values are accepted by the public
API.

### Supported option forms

```text
--port=8080       long option with a value
-p 8080           short option with a separate value
-p8080            short option with a compact value
--verbose         boolean option (equivalent to --verbose=true)
-abc              combined short boolean options
--                stop parsing options
```

Use `-h`, `--help`, or `help [command]` to display generated help.

## Positional arguments

Read positional arguments from `args()` inside or after a callback:

```cpp
for (const std::string &arg : command->args()) {
    std::cout << arg << '\n';
}
```

The positional argument list is cleared before every call to `execute`.
Arguments after `--` are always treated as positional.

## CMake integration

### Add as a subdirectory

```cmake
add_subdirectory(path/to/ccmd)
target_link_libraries(my_app PRIVATE ccmd::ccmd)
```

Examples are disabled automatically when `ccmd` is not the top-level project.

### Install and use `find_package`

Install the headers, `cflag`, and the CMake package files:

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCCMD_BUILD_EXAMPLES=OFF
cmake --build build --parallel
cmake --install build --prefix /path/to/prefix
```

Consume the installed package:

```cmake
find_package(ccmd CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE ccmd::ccmd)
```

Set `CMAKE_PREFIX_PATH` or pass `-Dccmd_DIR=...` when the installation prefix
is outside CMake's default search paths.

## Build and test

The convenience script covers the common development workflows:

```bash
./build.sh             # release build
./build.sh --debug     # debug build
./build.sh --test      # build and run all tests
./build.sh --cov       # test and generate cov/index.html
./build.sh --clean
```

The coverage command additionally requires `lcov` and `genhtml`.

Equivalent test commands:

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCCMD_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

The test build uses an installed GoogleTest package when available and
otherwise downloads the pinned test version.

### CMake options

| Option | Default | Purpose |
| --- | --- | --- |
| `CCMD_BUILD_EXAMPLES` | On for top-level builds | Build `ccmd-example` |
| `CCMD_BUILD_TESTS` | Off | Build the GoogleTest suite |
| `CCMD_FETCH_TEST_DEPS` | On | Fetch GoogleTest when it is unavailable |
| `CCMD_ENABLE_COVERAGE` | Off | Enable GCC/Clang coverage instrumentation |

## Error handling

Calling `execute` without a program name or adding a null subcommand throws
`std::invalid_argument`. Invalid options, values, duplicate names, or unknown
subcommands print a diagnostic and terminate with a non-zero exit status.
`-h`, `--help`, and valid `help` commands terminate successfully after printing
help.

## License

Apache License 2.0. See [LICENSE](LICENSE).
