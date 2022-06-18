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
    root_cmd->string_var("host", "127.0.0.1", "test server host ip address.");
    root_cmd->int_var("port", 9999, "test server port.");
    std::vector<std::string> arguments = {"test", "--host=0.0.0.0", "--port=10000"};
    root_cmd->execute(arguments);

    EXPECT_STREQ("0.0.0.0", root_cmd->string_var("host").c_str());
    EXPECT_EQ(10000, root_cmd->int_var("port"));
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
    c00->string_var("c00", "", "c00 command option.");

    std::shared_ptr<ccmd::c_command> c01 = std::make_shared<ccmd::c_command>(
        /* name       */ "c01",
        /* example    */ "c01 [options].",
        /* usage      */ "c01 [--c01=c01]",
        /* help_long  */ "this is a subcommand of test.",
        /* help_short */ "c01 command.",
        /* run        */ test_run 
    );
    c01->string_var("c01", "", "c01 command option.");
    root_cmd->add_subcommand(c00);
    root_cmd->add_subcommand(c01);

    std::vector<std::string> arguments = {"test", "c00", "--c00=c00"};
    root_cmd->execute(arguments);
    EXPECT_STREQ("c00", c00->string_var("c00").c_str());
    
    arguments = {"test", "c01", "--c01=c01"};
    root_cmd->execute(arguments);
    EXPECT_STREQ("c01", c01->string_var("c01").c_str());
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
    root_cmd->bool_varp("version", "v", false, "show command version.");
    std::vector<std::string> arguments = {"test", "--version=true", "arg00"}; 
    root_cmd->execute(arguments);

    EXPECT_EQ(1, root_cmd->args().size());
    EXPECT_STREQ("arg00", root_cmd->args().at(0).c_str());
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
    root_cmd->bool_varp("version", "v", false, "show command version.");
    std::shared_ptr<ccmd::c_command> sub_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "sub",
        /* example    */ "sub [--host=host].",
        /* usage      */ "sub [--host=host].",
        /* help_long  */ "this is a test sub command.",
        /* help_short */ "sub command.",
        /* run        */ test_run 
    );
    sub_cmd->string_var("host", "0.0.0.0", "set host value.");
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
    root_cmd->bool_varp("version", "v", false, "show command version.");
    std::shared_ptr<ccmd::c_command> sub_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "sub",
        /* example    */ "sub [--host=host].",
        /* usage      */ "sub [--host=host].",
        /* help_long  */ "this is a test sub command.",
        /* help_short */ "sub command.",
        /* run        */ test_run 
    );
    sub_cmd->string_var("host", "0.0.0.0", "set host value.");
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
    root_cmd->bool_varp("version", "v", false, "show command version.");
    std::shared_ptr<ccmd::c_command> sub_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "sub",
        /* example    */ "sub [--host=host].",
        /* usage      */ "sub [--host=host].",
        /* help_long  */ "this is a test sub command.",
        /* help_short */ "sub command.",
        /* run        */ test_run 
    );
    sub_cmd->bool_var("host", "0.0.0.0", "set host value.");
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
    root_cmd->bool_varp("version", "v", false, "show command version.");
    std::shared_ptr<ccmd::c_command> sub_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "sub",
        /* example    */ "sub [--host=host].",
        /* usage      */ "sub [--host=host].",
        /* help_long  */ "this is a test sub command.",
        /* help_short */ "sub command.",
        /* run        */ test_run 
    );
    sub_cmd->bool_var("host", "0.0.0.0", "set host value.");
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
    root_cmd->bool_varp("version", "v", false, "show command version.");
    std::shared_ptr<ccmd::c_command> sub_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "sub",
        /* example    */ "sub [--host=host].",
        /* usage      */ "sub [--host=host].",
        /* help_long  */ "this is a test sub command.",
        /* help_short */ "sub command.",
        /* run        */ test_run 
    );
    sub_cmd->bool_var("host", "0.0.0.0", "set host value.");
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
    root_cmd->bool_varp("verson", "v", false, "show command version.");

    std::vector<std::string> arguments = {"test", "help"};
    EXPECT_EXIT(root_cmd->execute(arguments), testing::ExitedWithCode(EXIT_FAILURE), ".*help.*");
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
    root_cmd->bool_varp("version", "v", false, "show command version.");
    std::shared_ptr<ccmd::c_command> sub_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "sub",
        /* example    */ "sub [--host=host].",
        /* usage      */ "sub [--host=host].",
        /* help_long  */ "this is a test sub command.",
        /* help_short */ "sub command.",
        /* run        */ test_run 
    );
    sub_cmd->bool_var("host", "0.0.0.0", "set host value.");
    root_cmd->add_subcommand(sub_cmd);

    std::vector<std::string> arguments = {"test", "help", "hello"}; 
    EXPECT_EXIT(root_cmd->execute(arguments), testing::ExitedWithCode(EXIT_FAILURE), ".*found.*");
}
