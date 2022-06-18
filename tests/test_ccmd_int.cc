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

TEST(test_int, test_int_flag) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [options].",
        /* usage      */ "test [-p --port=port] [--id=id].",
        /* help_long  */ "this is a int test command.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->int_varp("port", "p", 9999, "set server port.");
    root_cmd->int_var("id", 0, "set server id.");
    std::vector<std::string> arguments = {"test", "--port=10000", "--id=2"};
    root_cmd->execute(arguments);
    EXPECT_EQ(10000, root_cmd->int_var("port"));
    EXPECT_EQ(2, root_cmd->int_var("id"));

    arguments = {"test", "-p", "10001", "--id=3"};
    root_cmd->execute(arguments);
    EXPECT_EQ(10001, root_cmd->int_var("p"));
    EXPECT_EQ(3, root_cmd->int_var("id"));

    arguments = {"test", "-p10002", "--id=4"};
    root_cmd->execute(arguments);
    EXPECT_EQ(10002, root_cmd->int_var("p"));
    EXPECT_EQ(4, root_cmd->int_var("id"));
}

TEST(test_int, test_int_already_exist_flag) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [options].",
        /* usage      */ "test [-p --port=port].",
        /* help_long  */ "this is a int test command.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->int_varp("port", "p", 9999, "set server port.");
    EXPECT_EXIT(root_cmd->int_varp("port", "", 0, ""), testing::ExitedWithCode(EXIT_FAILURE), ".*exist.*");
    EXPECT_EXIT(root_cmd->int_varp("", "p", 0, ""), testing::ExitedWithCode(EXIT_FAILURE), ".*exist.*");
}

TEST(test_int, test_int_not_found_flag) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [options].",
        /* usage      */ "test [-p --port=port].",
        /* help_long  */ "this is a int test command.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->int_varp("port", "p", 9999, "set server port.");
    std::vector<std::string> arguments = {"test", "--port=10000"};
    root_cmd->execute(arguments);
    EXPECT_EXIT(root_cmd->int_var("hello"), testing::ExitedWithCode(EXIT_FAILURE), ".*found.*");
}
