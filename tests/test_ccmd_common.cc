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
#include <gtest/gtest.h>
#include <stdexcept>

void test_run(std::shared_ptr<ccmd::c_command> cmd) {
    return;
}

TEST(test_common, test_single_command) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [options].",
        /* usage      */ "test [--host=host] [--port=port].",
        /* help_long  */ "this is a single test command.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->var<std::string>("host", "127.0.0.1", "test server host ip address.");
    root_cmd->var<int>("port", 9999, "test server port.");
    std::vector<std::string> arguments = {"test", "--host=0.0.0.0", "--port=10000"};
    root_cmd->execute(arguments);

    EXPECT_EQ("0.0.0.0", root_cmd->var<std::string>("host"));
    EXPECT_EQ(10000, root_cmd->var<int>("port"));
}

TEST(test_common, test_multi_command) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [subcommand].",
        /* usage      */ "test [subcommand].",
        /* help_long  */ "this is a multi test command.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    std::shared_ptr<ccmd::c_command> c00 = std::make_shared<ccmd::c_command>(
        /* name       */ "c00",
        /* example    */ "c00 [options].",
        /* usage      */ "c00 [--c00=c00]",
        /* help_long  */ "this is a subcommand of test.",
        /* help_short */ "c00 command.",
        /* run        */ test_run 
    );
    c00->var<std::string>("c00", "", "c00 command option.");

    std::shared_ptr<ccmd::c_command> c01 = std::make_shared<ccmd::c_command>(
        /* name       */ "c01",
        /* example    */ "c01 [options].",
        /* usage      */ "c01 [--c01=c01]",
        /* help_long  */ "this is a subcommand of test.",
        /* help_short */ "c01 command.",
        /* run        */ test_run 
    );
    c01->var<std::string>("c01", "", "c01 command option.");
    root_cmd->add_subcommand(c00);
    root_cmd->add_subcommand(c01);

    std::vector<std::string> arguments = {"test", "c00", "--c00=c00"};
    root_cmd->execute(arguments);
    EXPECT_EQ("c00", c00->var<std::string>("c00"));
    
    arguments = {"test", "c01", "--c01=c01"};
    root_cmd->execute(arguments);
    EXPECT_EQ("c01", c01->var<std::string>("c01"));
}

TEST(test_common, test_cmd_exist) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [subcommand].",
        /* usage      */ "test [subcommand].",
        /* help_long  */ "this is a test sub command already exist command.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    std::shared_ptr<ccmd::c_command> c00 = std::make_shared<ccmd::c_command>(
        /* name       */ "c00",
        /* example    */ "c00 [options].",
        /* usage      */ "c00 [--c00=c00]",
        /* help_long  */ "this is a subcommand of test.",
        /* help_short */ "c00 command.",
        /* run        */ test_run 
    );
    std::shared_ptr<ccmd::c_command> c01 = std::make_shared<ccmd::c_command>(
        /* name       */ "c00",
        /* example    */ "c00 [options].",
        /* usage      */ "c00 [--c00=c00]",
        /* help_long  */ "this is a another subcommand of test.",
        /* help_short */ "c00 command.",
        /* run        */ test_run 
    );
    root_cmd->add_subcommand(c00);

    EXPECT_EXIT(root_cmd->add_subcommand(c01), testing::ExitedWithCode(EXIT_FAILURE), ".*exist.*");
}

TEST(test_common, test_cmd_args) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [-v --version=false].",
        /* usage      */ "test [-v --version=true].",
        /* help_long  */ "this is a test sub command args.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->varp<bool>("version", "v", false, "show command version.");
    std::vector<std::string> arguments = {"test", "--version=true", "arg00"}; 
    root_cmd->execute(arguments);

    EXPECT_EQ(1, root_cmd->args().size());
    EXPECT_STREQ("arg00", root_cmd->args().at(0).c_str());
}

TEST(test_common, test_repeated_execute_resets_positional_args) {
    auto root_cmd = std::make_shared<ccmd::c_command>(
        "test",
        "test [options].",
        "test [options].",
        "A command that can be executed more than once.",
        "test command.",
        test_run
    );

    std::vector<std::string> first_arguments = {"test", "first"};
    root_cmd->execute(first_arguments);
    ASSERT_EQ(1, root_cmd->args().size());
    EXPECT_EQ("first", root_cmd->args().front());

    const std::vector<std::string> second_arguments = {"test", "second"};
    root_cmd->execute(second_arguments);
    ASSERT_EQ(1, root_cmd->args().size());
    EXPECT_EQ("second", root_cmd->args().front());
}

TEST(test_common, test_const_string_api) {
    const std::string name = "test";
    const std::string example = "test [options].";
    const std::string usage = "test [--host=host].";
    const std::string help_long = "A const-friendly command.";
    const std::string help_short = "test command.";
    const std::string flag_name = "host";
    const std::string default_value = "127.0.0.1";
    const std::string flag_usage = "server address";
    const std::string count_name = "count";

    auto root_cmd = std::make_shared<ccmd::c_command>(
        name, example, usage, help_long, help_short, test_run);
    root_cmd->var<std::string>(flag_name, default_value, flag_usage);
    root_cmd->var<int>(count_name, 2, "item count");

    const std::vector<std::string> arguments = {"test", "--host=0.0.0.0"};
    root_cmd->execute(arguments);

    const ccmd::c_command &command = *root_cmd;
    EXPECT_EQ("0.0.0.0", command.var<std::string>(flag_name));
    EXPECT_EQ(2, command.var<int>(count_name));
    EXPECT_EQ(name, command.name());
}

TEST(test_common, test_invalid_execution_input) {
    auto root_cmd = std::make_shared<ccmd::c_command>(
        "test", "test.", "test.", "A test command.", "test command.");

    const std::vector<std::string> empty_arguments;
    EXPECT_THROW(root_cmd->execute(empty_arguments), std::invalid_argument);
    EXPECT_THROW(root_cmd->execute(0, nullptr), std::invalid_argument);
    EXPECT_THROW(root_cmd->add_subcommand(nullptr), std::invalid_argument);
    EXPECT_THROW(root_cmd->var<int>("", 0, ""), std::invalid_argument);
}

TEST(test_common, test_cmd_not_found) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [-v --version=false].",
        /* usage      */ "test [-v --version=true].",
        /* help_long  */ "this is a test sub command args.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->varp<bool>("version", "v", false, "show command version.");
    std::shared_ptr<ccmd::c_command> sub_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "sub",
        /* example    */ "sub [--host=host].",
        /* usage      */ "sub [--host=host].",
        /* help_long  */ "this is a test sub command.",
        /* help_short */ "sub command.",
        /* run        */ test_run 
    );
    sub_cmd->var<std::string>("host", "0.0.0.0", "set host value.");
    root_cmd->add_subcommand(sub_cmd);

    std::vector<std::string> arguments = {"test", "show", "--version=true", "arg00", "arg01"}; 
    EXPECT_EXIT(root_cmd->execute(arguments), testing::ExitedWithCode(EXIT_FAILURE), ".*found.*");
}

TEST(test_common, test_cmd_help_cmd) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [-v --version=false].",
        /* usage      */ "test [-v --version=true].",
        /* help_long  */ "this is a test sub command args.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->varp<bool>("version", "v", false, "show command version.");
    std::shared_ptr<ccmd::c_command> sub_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "sub",
        /* example    */ "sub [--host=host].",
        /* usage      */ "sub [--host=host].",
        /* help_long  */ "this is a test sub command.",
        /* help_short */ "sub command.",
        /* run        */ test_run 
    );
    sub_cmd->var<std::string>("host", "0.0.0.0", "set host value.");
    root_cmd->add_subcommand(sub_cmd);

    std::vector<std::string> arguments = {"test", "help", "sub"}; 
    EXPECT_EXIT(root_cmd->execute(arguments), testing::ExitedWithCode(0), "");
}

TEST(test_common, test_cmd_help_long_flag) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [-v --version=false].",
        /* usage      */ "test [-v --version=true].",
        /* help_long  */ "this is a test sub command args.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->varp<bool>("version", "v", false, "show command version.");
    std::shared_ptr<ccmd::c_command> sub_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "sub",
        /* example    */ "sub [--host=host].",
        /* usage      */ "sub [--host=host].",
        /* help_long  */ "this is a test sub command.",
        /* help_short */ "sub command.",
        /* run        */ test_run 
    );
    sub_cmd->var<std::string>("host", "0.0.0.0", "set host value.");
    root_cmd->add_subcommand(sub_cmd);

    std::vector<std::string> arguments = {"test", "sub", "--help"}; 
    EXPECT_EXIT(root_cmd->execute(arguments), testing::ExitedWithCode(0), "");
}

TEST(test_common, test_cmd_help_short_flag) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [-v --version=false].",
        /* usage      */ "test [-v --version=true].",
        /* help_long  */ "this is a test sub command args.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->varp<bool>("version", "v", false, "show command version.");
    std::shared_ptr<ccmd::c_command> sub_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "sub",
        /* example    */ "sub [--host=host].",
        /* usage      */ "sub [--host=host].",
        /* help_long  */ "this is a test sub command.",
        /* help_short */ "sub command.",
        /* run        */ test_run 
    );
    sub_cmd->var<std::string>("host", "0.0.0.0", "set host value.");
    root_cmd->add_subcommand(sub_cmd);

    std::vector<std::string> arguments = {"test", "sub", "-h"}; 
    EXPECT_EXIT(root_cmd->execute(arguments), testing::ExitedWithCode(0), "");
}

TEST(test_common, test_subcmd_help_short_flag) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [-v --version=false].",
        /* usage      */ "test [-v --version=true].",
        /* help_long  */ "this is a test sub command args.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->varp<bool>("version", "v", false, "show command version.");
    std::shared_ptr<ccmd::c_command> sub_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "sub",
        /* example    */ "sub [--host=host].",
        /* usage      */ "sub [--host=host].",
        /* help_long  */ "this is a test sub command.",
        /* help_short */ "sub command.",
        /* run        */ test_run 
    );
    sub_cmd->var<std::string>("host", "0.0.0.0", "set host value.");
    root_cmd->add_subcommand(sub_cmd);

    std::vector<std::string> arguments = {"test", "-h"}; 
    EXPECT_EXIT(root_cmd->execute(arguments), testing::ExitedWithCode(0), "");
}

TEST(test_common, test_subcmd_help_no_ccmd) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command> (
        /* name       */ "test",
        /* example    */ "test [-v --version=false].",
        /* usage      */ "test [-v --version=true].",
        /* help_long  */ "this is a test sub command args.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->varp<bool>("verson", "v", false, "show command version.");

    std::vector<std::string> arguments = {"test", "help"};
    EXPECT_EXIT(root_cmd->execute(arguments), testing::ExitedWithCode(EXIT_SUCCESS), "");
}

TEST(test_common, test_subcmd_help_not_found) {
    std::shared_ptr<ccmd::c_command> root_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "test",
        /* example    */ "test [-v --version=false].",
        /* usage      */ "test [-v --version=true].",
        /* help_long  */ "this is a test sub command args.",
        /* help_short */ "test command.",
        /* run        */ test_run 
    );
    root_cmd->varp<bool>("version", "v", false, "show command version.");
    std::shared_ptr<ccmd::c_command> sub_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "sub",
        /* example    */ "sub [--host=host].",
        /* usage      */ "sub [--host=host].",
        /* help_long  */ "this is a test sub command.",
        /* help_short */ "sub command.",
        /* run        */ test_run 
    );
    sub_cmd->var<std::string>("host", "0.0.0.0", "set host value.");
    root_cmd->add_subcommand(sub_cmd);

    std::vector<std::string> arguments = {"test", "help", "hello"}; 
    EXPECT_EXIT(root_cmd->execute(arguments), testing::ExitedWithCode(EXIT_FAILURE), ".*found.*");
}
