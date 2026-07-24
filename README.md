# ccmd

`ccmd` is a small command-line parser for C++11 and later. It supports nested
commands, long and short options, positional arguments, generated help, and
`bool`, `int`, `float`, and `string` values.

## Quick start

Clone the repository with its parser dependency:

```bash
git clone --recurse-submodules https://github.com/locallocal/ccmd.git
cd ccmd
./build.sh
./build/ccmd-example --help
```

Create commands with `std::make_shared` because callbacks receive the active
command as a `std::shared_ptr`:

```cpp
#include <ccmd.h>

#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
    auto root = std::make_shared<ccmd::c_command>(
        "server",
        "server start --port=8080",
        "server <command> [options]",
        "Manage the example server.",
        "server management"
    );

    auto start = std::make_shared<ccmd::c_command>(
        "start",
        "server start --port=8080",
        "server start [options]",
        "Start the server process.",
        "start the server",
        [](const std::shared_ptr<ccmd::c_command> &command) {
            std::cout << "port: " << command->int_var("port") << '\n';
        }
    );
    start->int_varp("port", "p", 8080, "server port");
    root->add_subcommand(start);

    root->execute(argc, argv);
}
```

Supported option forms:

```text
--port=8080       long option with a value
-p 8080           short option with a value
-p8080            compact short option
--verbose         boolean option (equivalent to --verbose=true)
-abc              combined short boolean options
--                stop parsing options
```

Use `-h`, `--help`, or `help [command]` to display generated help.

## Build and test

Requirements:

- A C++11-compatible compiler
- CMake 3.16 or newer
- Git submodules initialized with
  `git submodule update --init --recursive`

The convenience script covers the common workflows:

```bash
./build.sh             # release build
./build.sh --debug     # debug build
./build.sh --test      # build and run tests
./build.sh --cov       # test and generate cov/index.html
./build.sh --clean
```

The test command uses an installed GoogleTest package when available and
downloads the pinned test dependency otherwise.

Equivalent CMake commands:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel

cmake -S . -B build -DCCMD_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Available CMake options:

| Option | Default | Purpose |
| --- | --- | --- |
| `CCMD_BUILD_EXAMPLES` | On for top-level builds | Build `ccmd-example` |
| `CCMD_BUILD_TESTS` | Off | Build the GoogleTest suite |
| `CCMD_FETCH_TEST_DEPS` | On | Fetch GoogleTest if it is unavailable |
| `CCMD_ENABLE_COVERAGE` | Off | Enable GCC/Clang coverage instrumentation |

## Use from another CMake project

Install the library:

```bash
cmake -S . -B build -DCCMD_BUILD_EXAMPLES=OFF
cmake --build build --parallel
cmake --install build --prefix /path/to/prefix
```

Then consume the exported target:

```cmake
find_package(ccmd CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE ccmd::ccmd)
```

The project can also be included with `add_subdirectory`; examples are disabled
automatically when `ccmd` is not the top-level project.

## API overview

### Commands

```cpp
void execute(int argc, char *argv[]);
void execute(const std::vector<std::string> &arguments);
void add_subcommand(std::shared_ptr<c_command> command);
std::vector<std::string> &args();
void print_help();
```

`args()` contains positional arguments and is reset before every execution.
Invalid empty input throws `std::invalid_argument`.

### Options

Each value type has a registration function, a registration function with a
short name (the `p` suffix), and a getter:

```cpp
command->bool_varp("verbose", "v", false, "enable verbose logging");
command->int_varp("port", "p", 8080, "server port");
command->float_var("ratio", 0.5F, "sampling ratio");
command->string_varp("config", "c", "server.conf", "configuration file");

bool verbose = command->bool_var("verbose");
int port = command->int_var("p"); // long and short names both work
```

Both string literals and `const std::string` values are accepted throughout the
public API.

## License

Apache License 2.0. See [LICENSE](LICENSE).
