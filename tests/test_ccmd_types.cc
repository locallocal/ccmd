// Copyright 2022 locallocal
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include <cstdint>
#include <cstdio>
#include <fstream>
#include <string>
#include <vector>

#include "ccmd.h"
#include "test_ccmd.h"

// Tests for the value types that ccmd gained by delegating to
// cflag::flag_traits<T> (cflag >= v0.0.2), and for the built-in --flag-file.

namespace {

enum class mode { safe, fast };

}  // namespace

namespace cflag {

template <>
struct flag_traits<mode> {
    static const std::string& type_name() {
        static const std::string name = "mode";
        return name;
    }

    static std::string format(mode value) { return value == mode::safe ? "safe" : "fast"; }

    static bool parse(const std::string& text, mode& value) {
        if (text == "safe") {
            value = mode::safe;
            return true;
        }
        if (text == "fast") {
            value = mode::fast;
            return true;
        }
        return false;
    }

    static bool has_implicit_value() { return false; }
};

}  // namespace cflag

namespace {

std::shared_ptr<ccmd::command> make_command() {
    return std::make_shared<ccmd::command>(
        /* name       */ "test",
        /* example    */ "test [options].",
        /* usage      */ "test [options].",
        /* help_long  */ "this is a value type test command.",
        /* help_short */ "test command.",
        /* run        */ test_run);
}

class scoped_file {
public:
    scoped_file(const std::string& file_name, const std::string& content)
        : path_(testing::TempDir() + "ccmd_" + file_name) {
        std::ofstream output(path_.c_str(), std::ios::out | std::ios::binary);
        output << content;
    }

    ~scoped_file() { std::remove(path_.c_str()); }

    const std::string& path() const { return path_; }

private:
    std::string path_;
};

}  // namespace

TEST(test_types, test_double_flag) {
    auto root_cmd = make_command();
    root_cmd->varp("ratio", "r", 0.25, "sampling ratio.");
    EXPECT_DOUBLE_EQ(0.25, root_cmd->var<double>("ratio"));

    root_cmd->execute(std::vector<std::string>{"test", "--ratio=0.75"});
    EXPECT_DOUBLE_EQ(0.75, root_cmd->var<double>("ratio"));

    root_cmd->execute(std::vector<std::string>{"test", "-r", "1.5"});
    EXPECT_DOUBLE_EQ(1.5, root_cmd->var<double>("r"));
}

TEST(test_types, test_fixed_width_integer_flags) {
    auto root_cmd = make_command();
    root_cmd->var<std::int64_t>("offset", -1, "byte offset.");
    root_cmd->var<std::uint64_t>("size", 0, "byte size.");
    root_cmd->var<unsigned int>("workers", 1, "worker count.");

    root_cmd->execute(
        std::vector<std::string>{"test", "--offset=-4294967296", "--size=18446744073709551615", "--workers=8"});
    EXPECT_EQ(static_cast<std::int64_t>(-4294967296LL), root_cmd->var<std::int64_t>("offset"));
    EXPECT_EQ(static_cast<std::uint64_t>(18446744073709551615ULL), root_cmd->var<std::uint64_t>("size"));
    EXPECT_EQ(8U, root_cmd->var<unsigned int>("workers"));
}

TEST(test_types, test_integer_range_is_validated) {
    auto root_cmd = make_command();
    root_cmd->var<unsigned int>("workers", 1, "worker count.");
    EXPECT_EXIT(root_cmd->execute(std::vector<std::string>{"test", "--workers=-1"}),
                testing::ExitedWithCode(EXIT_FAILURE), ".*invalid value.*");

    root_cmd->var<short>("level", 0, "level.");
    EXPECT_EXIT(root_cmd->execute(std::vector<std::string>{"test", "--level=70000"}),
                testing::ExitedWithCode(EXIT_FAILURE), ".*invalid value.*");
}

TEST(test_types, test_custom_flag_traits) {
    auto root_cmd = make_command();
    root_cmd->varp("mode", "m", mode::safe, "execution mode.");
    EXPECT_EQ(mode::safe, root_cmd->var<mode>("mode"));

    root_cmd->execute(std::vector<std::string>{"test", "--mode=fast"});
    EXPECT_EQ(mode::fast, root_cmd->var<mode>("mode"));

    root_cmd->execute(std::vector<std::string>{"test", "-msafe"});
    EXPECT_EQ(mode::safe, root_cmd->var<mode>("m"));

    EXPECT_EXIT(root_cmd->execute(std::vector<std::string>{"test", "--mode=slow"}),
                testing::ExitedWithCode(EXIT_FAILURE), ".*invalid value.*");
}

TEST(test_types, test_reserved_flag_names) {
    auto root_cmd = make_command();
    EXPECT_EXIT(root_cmd->var<bool>("help", false, ""), testing::ExitedWithCode(EXIT_FAILURE), ".*reserved.*");
    EXPECT_EXIT(root_cmd->varp<bool>("verbose", "h", false, ""), testing::ExitedWithCode(EXIT_FAILURE), ".*reserved.*");
    EXPECT_EXIT(root_cmd->var<std::string>("flag-file", "", ""), testing::ExitedWithCode(EXIT_FAILURE), ".*reserved.*");
    EXPECT_EXIT(root_cmd->varp<bool>("verbose", "vv", false, ""), testing::ExitedWithCode(EXIT_FAILURE),
                ".*one character.*");
}

TEST(test_types, test_flag_file_json) {
    auto root_cmd = make_command();
    root_cmd->varp("port", "p", 9999, "server port.");
    root_cmd->var<std::string>("host", "127.0.0.1", "server host.");
    root_cmd->var("daemon", false, "run as daemon.");

    scoped_file file("flags.json", "{\"port\": 8080, \"host\": \"0.0.0.0\", \"daemon\": true}\n");
    root_cmd->execute(std::vector<std::string>{"test", "--flag-file=" + file.path(), "positional"});

    EXPECT_EQ(8080, root_cmd->var<int>("port"));
    EXPECT_EQ("0.0.0.0", root_cmd->var<std::string>("host"));
    EXPECT_TRUE(root_cmd->var<bool>("daemon"));
    ASSERT_EQ(1, root_cmd->args().size());
    EXPECT_EQ("positional", root_cmd->args().front());
}

TEST(test_types, test_flag_file_yaml_later_arguments_override) {
    auto root_cmd = make_command();
    root_cmd->varp("port", "p", 9999, "server port.");
    root_cmd->var<std::string>("host", "127.0.0.1", "server host.");

    scoped_file file("flags.yaml", "# comment\nport: 8080\nhost: 0.0.0.0\n");
    root_cmd->execute(std::vector<std::string>{"test", "--flag-file", file.path(), "--port=7070"});

    EXPECT_EQ(7070, root_cmd->var<int>("port"));
    EXPECT_EQ("0.0.0.0", root_cmd->var<std::string>("host"));
}

TEST(test_types, test_flag_file_in_subcommand) {
    auto root_cmd = make_command();
    auto sub_cmd = std::make_shared<ccmd::command>("sub", "sub [options].", "sub [options].",
                                                   "this is a subcommand of test.", "sub command.", test_run);
    sub_cmd->var("port", 1, "server port.");
    root_cmd->add_subcommand(sub_cmd);

    scoped_file file("flags.gflags", "--port=2\n");
    root_cmd->execute(std::vector<std::string>{"test", "sub", "--flag-file=" + file.path()});
    EXPECT_EQ(2, sub_cmd->var<int>("port"));
}

TEST(test_types, test_flag_file_missing) {
    auto root_cmd = make_command();
    root_cmd->var("port", 1, "server port.");
    EXPECT_EXIT(root_cmd->execute(std::vector<std::string>{"test", "--flag-file=/nonexistent/ccmd.flags"}),
                testing::ExitedWithCode(EXIT_FAILURE), ".*cannot open flag file.*");
}
