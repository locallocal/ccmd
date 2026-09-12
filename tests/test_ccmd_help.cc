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

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

#include "ccmd.h"
#include "test_ccmd.h"

namespace {

std::vector<std::string> split_lines(const std::string& text) {
    std::vector<std::string> lines;
    std::istringstream input(text);
    std::string line;
    while (std::getline(input, line)) {
        lines.push_back(line);
    }
    return lines;
}

std::vector<std::string> help_lines(const std::shared_ptr<ccmd::command>& cmd) {
    testing::internal::CaptureStdout();
    cmd->print_help();
    return split_lines(testing::internal::GetCapturedStdout());
}

// Lines that belong to the section headed by `header`, up to the next blank line.
std::vector<std::string> section(const std::vector<std::string>& lines, const std::string& header) {
    std::vector<std::string> rows;
    auto it = std::find(lines.begin(), lines.end(), header);
    if (it == lines.end()) {
        return rows;
    }
    for (++it; it != lines.end() && !it->empty(); ++it) {
        rows.push_back(*it);
    }
    return rows;
}

// Column of the description in a "  name  description" command row: the first
// non-space character after the gap that follows the name.
std::size_t description_column(const std::string& row) {
    const std::size_t label_begin = row.find_first_not_of(' ');
    const std::size_t gap = row.find("  ", label_begin);
    if (gap == std::string::npos) {
        return std::string::npos;
    }
    return row.find_first_not_of(' ', gap);
}

// Column of the description in a "  -x  --name[type]  description" option
// row: the first non-space character after the closing bracket of the type.
std::size_t option_description_column(const std::string& row) {
    const std::size_t bracket = row.find(']');
    if (bracket == std::string::npos) {
        return std::string::npos;
    }
    return row.find_first_not_of(' ', bracket + 1);
}

std::size_t longest_line(const std::vector<std::string>& lines) {
    std::size_t width = 0;
    for (const std::string& line : lines) {
        width = std::max(width, line.size());
    }
    return width;
}

}  // namespace

TEST(test_help, options_are_aligned_including_help_and_flag_file) {
    auto root_cmd = std::make_shared<ccmd::command>("test", "test --port=1.", "test [options].", "a help test.",
                                                    "test command.", test_run);
    root_cmd->varp("port", "p", 9999, "set server port.");
    root_cmd->var("verbose", false, "show verbose.");
    root_cmd->varp<std::string>("configuration-file", "c", "./test.conf", "configuration file.");

    const std::vector<std::string> options = section(help_lines(root_cmd), "Options:");
    ASSERT_EQ(5, options.size());
    EXPECT_NE(std::string::npos, options[0].find("--configuration-file[string]"));
    EXPECT_NE(std::string::npos, options[1].find("--flag-file[string]"));
    EXPECT_NE(std::string::npos, options[2].find("-h  --help[bool]"));
    EXPECT_NE(std::string::npos, options[3].find("-p  --port[int]"));
    EXPECT_NE(std::string::npos, options[3].find("(default: 9999)"));
    EXPECT_NE(std::string::npos, options[4].find("    --verbose[bool]"));

    const std::size_t column = option_description_column(options[0]);
    ASSERT_NE(std::string::npos, column);
    for (const std::string& row : options) {
        EXPECT_EQ(column, option_description_column(row)) << row;
    }
}

TEST(test_help, subcommands_are_aligned) {
    auto root_cmd =
        std::make_shared<ccmd::command>("test", "", "test <command>.", "a help test.", "test command.", test_run);
    root_cmd->add_subcommand(std::make_shared<ccmd::command>("a", "", "a.", "short name.", "the a command.", test_run));
    root_cmd->add_subcommand(std::make_shared<ccmd::command>("a-much-longer-name", "", "a-much-longer-name.",
                                                             "long name.", "the long command.", test_run));

    const std::vector<std::string> commands = section(help_lines(root_cmd), "Commands:");
    ASSERT_EQ(2, commands.size());
    EXPECT_EQ("  a                   the a command.", commands[0]);
    EXPECT_EQ("  a-much-longer-name  the long command.", commands[1]);
}

TEST(test_help, every_line_fits_in_100_columns) {
    const std::string long_text =
        "this description is deliberately far longer than one hundred columns so that the help printer has to "
        "wrap it onto several lines while keeping the continuation lines aligned with the first one.";
    auto root_cmd = std::make_shared<ccmd::command>(
        "test", "test sub --port=1 --host=127.0.0.1 --conf=/an/extremely/long/path/to/a/configuration/file.conf x y",
        "test [subcommand] [--port=port] [--host=host] [--conf=conf] [--verbose] [--flag-file=file] [args...]",
        long_text, "test command.", test_run);
    root_cmd->varp("port", "p", 9999, long_text);
    root_cmd->var<std::string>("host", "127.0.0.1", "host address.");
    root_cmd->add_subcommand(std::make_shared<ccmd::command>("sub", "", "sub.", long_text, long_text, test_run));

    const std::vector<std::string> lines = help_lines(root_cmd);
    EXPECT_LE(longest_line(lines), 100U);
    for (const std::string& line : lines) {
        EXPECT_LE(line.size(), 100U) << line;
    }

    // Wrapped continuation lines of an option keep the description column.
    const std::vector<std::string> options = section(lines, "Options:");
    ASSERT_GT(options.size(), 4U);
    const std::size_t column = option_description_column(options[0]);
    for (const std::string& row : options) {
        if (row.find("--") != std::string::npos) {
            EXPECT_EQ(column, option_description_column(row)) << row;
        } else {
            EXPECT_EQ(column, row.find_first_not_of(' ')) << row;
        }
    }
    const std::vector<std::string> commands = section(lines, "Commands:");
    ASSERT_GT(commands.size(), 1U);
    EXPECT_EQ(description_column(commands[0]), commands[1].find_first_not_of(' '));
}

TEST(test_help, custom_type_and_empty_default_are_rendered) {
    auto root_cmd = std::make_shared<ccmd::command>("test", "", "test.", "a help test.", "test command.", test_run);
    root_cmd->var("ratio", 0.5, "sampling ratio.");
    root_cmd->var<std::string>("name", "", "optional name.");

    const std::vector<std::string> options = section(help_lines(root_cmd), "Options:");
    bool saw_ratio = false;
    bool saw_name = false;
    for (const std::string& row : options) {
        if (row.find("--ratio[double]") != std::string::npos) {
            saw_ratio = true;
            EXPECT_NE(std::string::npos, row.find("(default: 0.5"));
        }
        if (row.find("--name[string]") != std::string::npos) {
            saw_name = true;
            EXPECT_EQ(std::string::npos, row.find("default"));
            EXPECT_NE(std::string::npos, row.find("optional name."));
        }
    }
    EXPECT_TRUE(saw_ratio);
    EXPECT_TRUE(saw_name);
}
