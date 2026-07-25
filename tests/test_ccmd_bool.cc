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

#include "ccmd.h"
#include "test_ccmd.h"
#include <gtest/gtest.h>

TEST(test_bool, test_bool_flag) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [options].",
        /* usage      */ "test [-v --version=false].",
        /* help_long  */ "this is a bool flag test command.",
        /* help_short */ "test command.",
        /* run        */ test_run
    );
    root_cmd->varp("version", "v", false, "show test command version.");
    root_cmd->var("verbose", false, "show verbose.");
    std::vector<std::string> arguments = {"test", "--version=true", "--verbose=true"};
    root_cmd->execute(arguments);
    EXPECT_TRUE(root_cmd->var<bool>("version"));
    EXPECT_TRUE(root_cmd->var<bool>("verbose"));

    arguments = {"test", "--version=false", "--verbose=false"};
    root_cmd->execute(arguments);
    EXPECT_FALSE(root_cmd->var<bool>("version"));
    EXPECT_FALSE(root_cmd->var<bool>("v"));
    EXPECT_FALSE(root_cmd->var<bool>("verbose"));

    arguments = {"test", "-v"};
    root_cmd->execute(arguments);
    EXPECT_TRUE(root_cmd->var<bool>("version"));
    EXPECT_TRUE(root_cmd->var<bool>("v"));
}

TEST(test_bool, test_bool_already_exist_flag) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [options].",
        /* usage      */ "test [-v --version=false].",
        /* help_long  */ "this is a bool flag test command.",
        /* help_short */ "test command.",
        /* run        */ test_run
    );
    root_cmd->varp<bool>("version", "v", false, "show test command version.");
    EXPECT_EXIT(root_cmd->varp<bool>("version", "", false, ""),
        testing::ExitedWithCode(EXIT_FAILURE), ".*exist.*");
    EXPECT_EXIT(root_cmd->varp<bool>("", "v", false, ""),
        testing::ExitedWithCode(EXIT_FAILURE), ".*exist.*");
}

TEST(test_bool, test_bool_not_found_flag) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [options].",
        /* usage      */ "test [-v --version=false].",
        /* help_long  */ "this is a bool flag test command.",
        /* help_short */ "test command.",
        /* run        */ test_run
    );
    root_cmd->varp<bool>("version", "v", false, "show test command version.");
    std::vector<std::string> arguments = {"test", "--version=true"};
    root_cmd->execute(arguments);
    EXPECT_EXIT(root_cmd->var<bool>("hello"), testing::ExitedWithCode(EXIT_FAILURE), ".*found.*");
}

TEST(test_bool, test_flag_type_mismatch) {
    auto root_cmd = std::make_shared<ccmd::c_command>(
        "test", "test [options].", "test [options].", "test command.", "test command.");
    root_cmd->var<bool>("verbose", false, "show verbose.");

    EXPECT_EXIT(root_cmd->var<int>("verbose"),
        testing::ExitedWithCode(EXIT_FAILURE), ".*different type.*");
}
