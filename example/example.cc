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

std::shared_ptr<ccmd::c_command> register_commands();
void add_master_command(std::shared_ptr<ccmd::c_command> cmd);
void add_meta_command(std::shared_ptr<ccmd::c_command> cmd);
void add_storage_command(std::shared_ptr<ccmd::c_command> cmd);
void root_run(std::shared_ptr<ccmd::c_command> cmd);
void master_run(std::shared_ptr<ccmd::c_command> cmd);
void meta_run(std::shared_ptr<ccmd::c_command> cmd);
void storage_run(std::shared_ptr<ccmd::c_command> cmd);

int main(int argc, char *argv[]) {
    std::shared_ptr<ccmd::c_command> root_cmd = register_commands();
    root_cmd->execute(argc, argv);
    return 0;
}

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

    add_master_command(root_cmd);
    add_meta_command(root_cmd);
    add_storage_command(root_cmd);
    return root_cmd;
}

void root_run(std::shared_ptr<ccmd::c_command> cmd) {
    std::cout << "this is a example." << std::endl;
}

void add_master_command(std::shared_ptr<ccmd::c_command> cmd) {
    std::shared_ptr<ccmd::c_command> master_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "master",
        /* example    */ "example master --conf=./master.conf --port=9999.",
        /* usage      */ "master [--conf=./master.conf] [--port=port] [-d/--daemon].",
        /* help_long  */ "start a master server.",
        /* help_short */ "start a master server.",
        /* run        */ master_run
    );
    master_cmd->bool_varp("daemon", "d", false, "run daemon.");
    master_cmd->int_varp("port", "p", 9999, "set server port.");
    master_cmd->string_varp("conf", "c", "", "configuration file.");
    cmd->add_subcommand(master_cmd);
}

void add_meta_command(std::shared_ptr<ccmd::c_command> cmd) {
    std::shared_ptr<ccmd::c_command> meta_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "meta",
        /* example    */ "example meta --conf=./meta.conf --port=10000.",
        /* usage      */ "meta [--master=127.0.0.1:9999] [--conf=./meta.conf] [--port=port] [-d/--daemon].",
        /* help_long  */ "start a meta server.",
        /* help_short */ "start a meta server.",
        /* run        */ meta_run
    );
    meta_cmd->bool_varp("daemon", "d", false, "run daemon.");
    meta_cmd->int_varp("port", "p", 9998, "set server port.");
    meta_cmd->string_varp("master", "m", "0.0.0.0", "master address.");
    meta_cmd->string_varp("conf", "c", "./meta.conf", "configuration file.");
    cmd->add_subcommand(meta_cmd);
}

void add_storage_command(std::shared_ptr<ccmd::c_command> cmd) {
    std::shared_ptr<ccmd::c_command> storage_cmd = std::make_shared<ccmd::c_command>(
        /* name       */ "storage",
        /* example    */ "example storage --conf=./storage.conf --port=100001.",
        /* usage      */ "storage [--master=127.0.0.1:9999] [--conf=./storage.conf] [--port=port] [-d/--daemon].",
        /* help_long  */ "start a storage server.",
        /* help_short */ "start a storage server.",
        /* run        */ storage_run
    );
    storage_cmd->bool_varp("daemon", "d", false, "run daemon.");
    storage_cmd->int_varp("port", "p", 9997, "set server port.");
    storage_cmd->string_varp("master", "m", "0.0.0.0", "master address.");
    storage_cmd->string_varp("conf", "c", "./storage.conf", "configuration file.");
    cmd->add_subcommand(storage_cmd);
}

void master_run(std::shared_ptr<ccmd::c_command> cmd) {
    for (auto &arg : cmd->args()) {
        std::cout << arg << " ";
    }
    std::cout << std::endl;
    std::cout << "daemon: " << std::boolalpha << cmd->bool_var("daemon") << std::endl;
    std::cout << "port: " << cmd->int_var("port") << std::endl;
    std::cout << "conf: " << cmd->string_var("conf") << std::endl;
}

void meta_run(std::shared_ptr<ccmd::c_command> cmd) {
    for (auto &arg : cmd->args()) {
        std::cout << arg << " ";
    }
    std::cout << std::endl;
    std::cout << "daemon: " << std::boolalpha << cmd->bool_var("daemon") << std::endl;
    std::cout << "port: " << cmd->int_var("port") << std::endl;
    std::cout << "master: " << cmd->string_var("master") << std::endl;
    std::cout << "conf: " << cmd->string_var("conf") << std::endl;
}

void storage_run(std::shared_ptr<ccmd::c_command> cmd) {
    for (auto &arg : cmd->args()) {
        std::cout << arg << " ";
    }
    std::cout << std::endl;
    std::cout << "daemon: " << std::boolalpha << cmd->bool_var("daemon") << std::endl;
    std::cout << "port: " << cmd->int_var("port") << std::endl;
    std::cout << "master: " << cmd->string_var("master") << std::endl;
    std::cout << "conf: " << cmd->string_var("conf") << std::endl;
}
