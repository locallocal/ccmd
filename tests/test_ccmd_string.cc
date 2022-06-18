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

TEST(test_string, test_string_flag) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [options].",
        /* usage      */ "test [-m --master=master] [--host=host].",
        /* help_long  */ "this is a string test command.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->string_varp("master", "m", "0.0.0.0", "master server address.");
    root_cmd->string_var("host", "0.0.0.0", "host addresss.");
    std::vector<std::string> arguments = {"test", "--master=127.0.0.1", "--host=1.0.0.1"};
    root_cmd->execute(arguments);
    EXPECT_STREQ("127.0.0.1", root_cmd->string_var("master").c_str());
    EXPECT_STREQ("1.0.0.1", root_cmd->string_var("host").c_str());

    arguments = {"test", "-m", "127.0.0.2", "--host=1.0.0.2"};
    root_cmd->execute(arguments);
    EXPECT_STREQ("127.0.0.2", root_cmd->string_var("master").c_str());
    EXPECT_STREQ("1.0.0.2", root_cmd->string_var("host").c_str());

    arguments = {"test", "-m127.0.0.3"};
    root_cmd->execute(arguments);
    EXPECT_STREQ("127.0.0.3", root_cmd->string_var("master").c_str());
}

TEST(test_string, test_string_already_exist_flag) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [options].",
        /* usage      */ "test [-m --master=master].",
        /* help_long  */ "this is a string test command.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->string_varp("master", "m", "0.0.0.0", "master server address.");
    EXPECT_EXIT(root_cmd->string_varp("master", "", "", ""), testing::ExitedWithCode(EXIT_FAILURE), ".*exist.*");
    EXPECT_EXIT(root_cmd->string_varp("", "m", "", ""), testing::ExitedWithCode(EXIT_FAILURE), ".*exist.*");
}

TEST(test_string, test_string_not_found_flag) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [options].",
        /* usage      */ "test [-m --master=master].",
        /* help_long  */ "this is a string test command.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->string_varp("master", "m", "0.0.0.0", "master server address.");
    std::vector<std::string> arguments = {"test", "--master=127.0.0.1"};
    root_cmd->execute(arguments);
    EXPECT_EXIT(root_cmd->string_var("hello"), testing::ExitedWithCode(EXIT_FAILURE), ".*found.*");
}


