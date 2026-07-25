# ccmd

[English](../README.md)

`ccmd` 是一个支持 C++11 及以上版本的 header-only 命令行框架。它在
[`cflag`](https://github.com/locallocal/cflag) 的参数解析和类型转换能力之上，
提供命令、嵌套子命令、回调函数以及自动生成的帮助信息。

## 功能特性

- header-only 的 `ccmd` API 和 CMake 接口目标
- 支持命令和任意层级的嵌套子命令
- 支持长选项和短选项
- 支持 `bool`、`int`、`float` 和 `string` 类型的选项值
- 支持位置参数和 `--` 选项终止符
- 支持命令回调和自动生成帮助信息
- 公共 API 兼容 C++11

## 环境要求

- 支持 C++11 的编译器
- CMake 3.16 或更高版本
- `cflag` Git 子模块

克隆仓库时可以同时初始化依赖：

```bash
git clone --recurse-submodules https://github.com/locallocal/ccmd.git
cd ccmd
```

如果仓库已经克隆，可以执行：

```bash
git submodule update --init --recursive
```

`ccmd` 本身不会生成库文件。导出的 `ccmd::ccmd` 是一个 CMake 接口目标，
它会向使用方传递所需的头文件路径，并自动链接编译后的 `cflag` 依赖。

## 快速开始

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

构建并运行仓库中的示例：

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
./build/ccmd-example --help
```

请使用 `std::make_shared` 创建命令对象。命令回调内部会调用
`shared_from_this()`，因此命令必须由 `std::shared_ptr` 管理。

## 子命令

通过 `add_subcommand` 注册子命令：

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

子命令可以继续包含自己的子命令。每个命令拥有独立的选项集合，并且只接收
属于自己的位置参数。

## 选项

每一种受支持的值类型都提供三种操作：

- `*_var(name, default_value, usage)`：注册长选项。
- `*_varp(name, short_name, default_value, usage)`：为同一个选项同时注册长名称
  和短名称。
- `*_var(name)`：读取解析后的值。

```cpp
command->bool_varp("verbose", "v", false, "enable verbose logging");
command->int_varp("port", "p", 8080, "server port");
command->float_var("ratio", 0.5F, "sampling ratio");
command->string_varp("config", "c", "server.conf", "configuration file");

bool verbose = command->bool_var("verbose");
int port = command->int_var("p"); // 长名称和短名称均可用于读取。
float ratio = command->float_var("ratio");
const std::string &config = command->string_var("config");
```

公共 API 同时接受字符串字面量和 `const std::string`。

### 支持的选项形式

```text
--port=8080       带值的长选项
-p 8080           短选项和值分开传递
-p8080            短选项和值紧凑传递
--verbose         布尔选项，等价于 --verbose=true
-abc              合并多个布尔短选项
--                停止解析选项
```

使用 `-h`、`--help` 或 `help [command]` 显示自动生成的帮助信息。

## 位置参数

可以在回调函数内或命令执行后通过 `args()` 读取位置参数：

```cpp
for (const std::string &arg : command->args()) {
    std::cout << arg << '\n';
}
```

每次调用 `execute` 前都会清空已有的位置参数。`--` 之后的所有参数都会被
视为位置参数。

## CMake 集成

### 通过子目录引入

```cmake
add_subdirectory(path/to/ccmd)
target_link_libraries(my_app PRIVATE ccmd::ccmd)
```

当 `ccmd` 不是顶层项目时，示例程序默认不会构建。

### 安装后通过 `find_package` 使用

安装头文件、`cflag` 以及 CMake 包配置：

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DCCMD_BUILD_EXAMPLES=OFF
cmake --build build --parallel
cmake --install build --prefix /path/to/prefix
```

在使用方项目中加载安装后的包：

```cmake
find_package(ccmd CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE ccmd::ccmd)
```

如果安装目录不在 CMake 的默认搜索路径中，请设置 `CMAKE_PREFIX_PATH`，
或者通过 `-Dccmd_DIR=...` 指定配置文件目录。

## 构建与测试

项目提供了覆盖常用开发流程的构建脚本：

```bash
./build.sh             # Release 构建
./build.sh --debug     # Debug 构建
./build.sh --test      # 构建并运行全部测试
./build.sh --cov       # 运行测试并生成 cov/index.html
./build.sh --clean
```

生成覆盖率报告还需要安装 `lcov` 和 `genhtml`。

等价的测试命令：

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCCMD_BUILD_TESTS=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

测试构建会优先使用系统中已安装的 GoogleTest；如果未找到，则下载项目固定
版本的 GoogleTest。

### CMake 选项

| 选项 | 默认值 | 用途 |
| --- | --- | --- |
| `CCMD_BUILD_EXAMPLES` | 顶层构建时开启 | 构建 `ccmd-example` |
| `CCMD_BUILD_TESTS` | 关闭 | 构建 GoogleTest 测试套件 |
| `CCMD_FETCH_TEST_DEPS` | 开启 | 找不到 GoogleTest 时自动下载 |
| `CCMD_ENABLE_COVERAGE` | 关闭 | 启用 GCC/Clang 覆盖率插桩 |

## 错误处理

调用 `execute` 时没有传入程序名称，或者添加空的子命令，会抛出
`std::invalid_argument`。无效选项、无效值、重复名称或未知子命令会输出诊断
信息，并以非零状态码退出。`-h`、`--help` 和有效的 `help` 命令在输出帮助
信息后会正常退出。

## 许可证

本项目使用 Apache License 2.0，详见 [LICENSE](../LICENSE)。
