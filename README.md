# ccmd

[简体中文](docs/README_zh-CN.md)

`ccmd` is a header-only command-line framework for C++11 and later. It adds
commands, nested subcommands, callbacks, and generated help on top of
[`cflag`](https://github.com/locallocal/cflag), which performs option parsing
and value conversion. A single `var<T>` / `varp<T>` template API covers all
supported value types.

## Features

- Header-only `ccmd` API with a CMake interface target
- Commands and arbitrarily nested subcommands
- Long and short options
- Type-safe template flags for `bool`, every integer and floating-point type,
  `std::string`, and custom types through `cflag::flag_traits<T>`
- Flag files in JSON, YAML, or gflags format via the built-in `--flag-file`
- Positional arguments and the `--` option terminator
- Command callbacks and generated help
- C++11-compatible public API

## Requirements

- A C++11-compatible compiler
- CMake 3.16 or newer
- The `cflag` Git submodule (v0.0.2 or newer, header-only)

Clone the repository and initialize the dependency in one step:

```bash
git clone --recurse-submodules https://github.com/locallocal/ccmd.git
cd ccmd
```

For an existing clone, initialize the dependency with:

```bash
git submodule update --init --recursive
```

Both `ccmd` and `cflag` are header-only, so nothing is compiled or linked.
The exported `ccmd::ccmd` interface target supplies its own headers and the
`cflag` include path transitively.

## Quick start

```cpp
#include <ccmd.h>

#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    auto command = std::make_shared<ccmd::command>(
        "server",
        "server --port=8080",
        "server [options]",
        "Start the example server.",
        "start a server",
        [](const std::shared_ptr<ccmd::command> &active) {
            std::cout << "verbose: " << std::boolalpha
                      << active->var<bool>("verbose") << '\n';
            std::cout << "port: " << active->var<int>("port") << '\n';
        }
    );

    command->varp("verbose", "v", false, "enable verbose output");
    command->varp("port", "p", 8080, "server port");
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
auto root = std::make_shared<ccmd::command>(
    "server",
    "server start --port=8080",
    "server <command> [options]",
    "Manage the server.",
    "server management"
);

auto start = std::make_shared<ccmd::command>(
    "start",
    "server start --port=8080",
    "server start [options]",
    "Start the server process.",
    "start the server",
    [](const std::shared_ptr<ccmd::command> &active) {
        std::cout << "port: " << active->var<int>("port") << '\n';
    }
);

start->varp("port", "p", 8080, "server port");
root->add_subcommand(start);
root->execute(argc, argv);
```

Subcommands can contain their own subcommands. Each command owns an independent
option set and receives only its own positional arguments.

## Options

Flags use the following template operations:

- `var<T>(name, default_value, usage)` registers a long option.
- `varp<T>(name, short_name, default_value, usage)` registers long and short
  names for the same option.
- `var<T>(name)` returns the parsed value and verifies its registered type.

```cpp
command->varp("verbose", "v", false, "enable verbose logging");
command->varp("port", "p", 8080, "server port");
command->var("ratio", 0.5F, "sampling ratio");
command->varp<std::string>(
    "config", "c", "server.conf", "configuration file");

bool verbose = command->var<bool>("verbose");
int port = command->var<int>("p"); // Long and short names both work.
float ratio = command->var<float>("ratio");
std::string config = command->var<std::string>("config");
```

Registration templates infer `T` from the default value. Specify
`std::string` explicitly when the default is a string literal. Conversion and
validation are delegated to `cflag::flag_traits<T>`, so every type that
`cflag` supports is available: `bool`, all character, integer, and
floating-point types (including the fixed-width `<cstdint>` aliases),
`std::nullptr_t`, and `std::string`. Integer values are range-checked and
partial parses are rejected. Registering a type without a `flag_traits`
specialization produces a compile-time error.

The names `help`, `h`, and `flag-file` are reserved by `cflag`; registering
them terminates with a diagnostic.

### Custom value types

Specialize `cflag::flag_traits<T>` to register your own type with `var<T>` /
`varp<T>`:

```cpp
enum class mode { safe, fast };

namespace cflag {
template <>
struct flag_traits<mode> {
    static const std::string &type_name() {
        static const std::string name = "mode";
        return name;
    }
    static std::string format(mode value) { return value == mode::safe ? "safe" : "fast"; }
    static bool parse(const std::string &text, mode &value) {
        if (text == "safe") { value = mode::safe; return true; }
        if (text == "fast") { value = mode::fast; return true; }
        return false; // leave value untouched
    }
    static bool has_implicit_value() { return false; }
};
} // namespace cflag

command->varp("mode", "m", mode::safe, "execution mode");
mode execution_mode = command->var<mode>("mode");
```

### Supported option forms

```text
--port=8080       long option with a value
-p 8080           short option with a separate value
-p8080            short option with a compact value
--verbose         boolean option (equivalent to --verbose=true)
-abc              combined short boolean options
--flag-file=FILE  load options from a file (see below)
--                stop parsing options
```

Use `-h`, `--help`, or `help [command]` to display generated help.

### Flag files

Every command accepts `--flag-file=<path>`. The file is applied at that
position on the command line, so later arguments override it, and files may
include other files with `flag-file` (16 levels deep at most). The format is
chosen by extension (`.json`, `.yaml`, `.yml`) and otherwise sniffed from the
content:

```yaml
# server.yaml
port: 8080
verbose: true
```

```json
{"port": 8080, "verbose": true}
```

```text
# server.gflags
--port=8080
--verbose
```

Only flat key/value content is accepted; nested objects, lists, and `null`
are rejected with a diagnostic. To apply a file programmatically, call
`command->flag_set()->parse_file(path)`.

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

Install the headers and the CMake package files:

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

Calling `execute` without a program name, adding a null subcommand, or
registering a flag without either a long or short name throws
`std::invalid_argument`. Invalid options, values, duplicate or reserved names,
multi-character short names, type mismatches, unreadable flag files, or unknown
subcommands print a diagnostic and terminate with a non-zero exit status.
`-h`, `--help`, and valid `help` commands terminate successfully after printing
help.

## License

Apache License 2.0. See [LICENSE](LICENSE).
