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

TEST(test_float, test_float_flag) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [options].",
        /* usage      */ "test [-p --percentage=percentage] [--point=point].",
        /* help_long  */ "this is a float test command.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->float_varp("percentage", "p", 0.0, "set percentage.");
    root_cmd->float_var("point", 0.0, "set point.");
    std::vector<std::string> arguments = {"test", "--percentage=0.8", "--point=0.8"};
    root_cmd->execute(arguments);
    EXPECT_FLOAT_EQ(0.8, root_cmd->float_var("percentage"));
    EXPECT_FLOAT_EQ(0.8, root_cmd->float_var("point"));

    arguments = {"test", "-p", "0.7"};
    root_cmd->execute(arguments);
    EXPECT_FLOAT_EQ(0.7, root_cmd->float_var("p"));

    arguments = {"test", "-p0.5"};
    root_cmd->execute(arguments);
    EXPECT_FLOAT_EQ(0.5, root_cmd->float_var("p"));
}

TEST(test_float, test_float_already_exist_flag) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [options].",
        /* usage      */ "test [-p --percentage=percentage].",
        /* help_long  */ "this is a float test command.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->float_varp("percentage", "p", 0.0, "set percentage.");
    EXPECT_EXIT(root_cmd->float_varp("percentage", "", 0.1, ""), testing::ExitedWithCode(EXIT_FAILURE), ".*exist.*");
    EXPECT_EXIT(root_cmd->float_varp("", "p", 0.2, ""), testing::ExitedWithCode(EXIT_FAILURE), ".*exist.*");
}

TEST(test_float, test_float_not_found_flag) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [options].",
        /* usage      */ "test [-p --percentage=percentage].",
        /* help_long  */ "this is a float test command.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->float_varp("percentage", "p", 0.0, "set percentage.");
    std::vector<std::string> arguments = {"test", "--percentage=0.8"};
    root_cmd->execute(arguments);
    EXPECT_EXIT(root_cmd->float_var("hello"), testing::ExitedWithCode(EXIT_FAILURE), ".*found.*");
}
