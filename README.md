# ccmd
ccmd is a command line parser for C++11 and beyond that provides a rich feature set with a simple and intuitive interface.

# 1.How to use
```
std::shared_ptr<ccmd::c_command> register_commands() {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "example",
        /* example    */ "example meta [options].",
        /* usage      */ "example [subcommand] [options].",
        /* help_long  */ "example is a tutorial of ccmd.",
        /* help_short */ "example command.",
        /* run        */ root_run
    );
    root_cmd->bool_varp("version", "v", false, "show version.");
    root_cmd->bool_var("verbose", false, "show verbose.");

    return root_cmd;
}
```

Run example after buiding.
```
# ./build/bin/ccmd-example --help
command example:
  example is a tutorial of ccmd.

example:
  example meta [options].

usage:
  example [subcommand] [options].

subcommands:
  master	start a master server.
  meta	start a meta server.
  storage	start a storage server.

options:
     --verbose[bool]	show verbose.(false)
 -v  --version[bool]	show version.(false)
```

# 2.How to build
- build release or debug.
```
# ./build.sh
# ./build.sh --debug
```

- run tests after build.
```
./build.sh --test
```

- run source code coverage analysis after run tests.
```
# ./build.sh --cov
# ll cov
```

- clean directory.
```
# ./build.sh --clean
```


# 3.API
## 3.1 cmd 
- `void execute(int argc, char *argv[]);`

- `void execute(std::vector<std::string> &arguments);`

- `void add_subcommand(std::shared_ptr<c_command> cmd);`

- `void print_help();`

- `void print_sub_command();`

- `void print_flag_set();`

## 3.2 bool flag
- `bool bool_var(const char *name);`

- `bool bool_var(std::string &name);`

- `void bool_var(const char *name, bool default_value, const char *usage);`

- `void bool_varp(const char *name, const char *short_name, bool default_value, const char *usage);`

- `void bool_var(std::string &name, bool default_value, std::string &usage);`

- `void bool_varp(std::string &name, std::string &short_name, bool default_value, std::string &usage);`
 

## 3.3 int flag
- `int int_var(const char *name);`

- `int int_var(std::string &name);`

- `void int_var(const char *name, int default_value, const char *usage);`

- `void int_varp(const char *name, const char *short_name, int default_value, const char *usage);`

- `void int_var(std::string &name, int default_value, std::string &usage);`

- `void int_varp(std::string &name, std::string &short_name, int default_value, std::string &usage);`

## 3.4 float flag
- `float float_var(const char *name);`

- `float float_var(std::string &name);`

- `void float_var(const char *name, float default_value, const char *usage);`

- `void float_varp(const char *name, const char *short_name, float default_value, const char *usage);`

- `void float_var(std::string &name, float default_value, std::string &usage);`

- `void float_varp(std::string &name, std::string &short_name, float default_value, std::string &usage);`
 

## 3.5 string flag
- `std::string &string_var(const char *name);`

- `std::string &string_var(std::string &name);`

- `void string_var(const char *name, const char *default_value, const char *usage);`

- `void string_varp(const char *name, const char *short_name, const char *default_value, const char *usage);`

- `void string_var(std::string &name, std::string &default_value, std::string &usage);`

- `void string_varp(std::string &name, std::string &short_name, std::string &default_value, std::string &usage);`

