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

const std::string ccmd::k_help_command_name = "help";
const std::string ccmd::k_help_flag_long_name = "--help";
const std::string ccmd::k_help_flag_short_name = "-h";

ccmd::c_command::c_command(const char *name, const char *example, const char *usage, const char *help_long,
        const char *help_short, std::function<void(std::shared_ptr<ccmd::c_command>)> run) {
    name_ = name;
    example_ = example;
    usage_ = usage;
    help_long_ = help_long;
    help_short_ = help_short;
    run_ = run;
    flag_set_ = std::make_shared<cflag::c_flag_set>(name_);
}

ccmd::c_command::c_command(std::string &name, std::string &example, std::string &usage, std::string &help_long,
        std::string &help_short, std::function<void(std::shared_ptr<ccmd::c_command>)> run) {
    name_ = name;
    example_ = example;
    usage_ = usage;
    help_long_ = help_long;
    help_short_ = help_short;
    run_ = run;
    flag_set_ = std::make_shared<cflag::c_flag_set>(name);
}

std::vector<std::string> &ccmd::c_command::args() {
    return flag_set_->args();
}

void ccmd::c_command::execute(int argc, char *argv[]) {
    std::vector<std::string> arguments;
    for(int i = 0; i < argc; i++) {
        arguments.push_back(argv[i]);
    }
    execute(arguments);
}

void ccmd::c_command::execute(std::vector<std::string> &arguments) {
    parse_(arguments);
}

void ccmd::c_command::add_subcommand(std::shared_ptr<ccmd::c_command> cmd) {
    auto it = sub_commands_.find(cmd->name());
    if (it != sub_commands_.end()) {
        std::cerr << "sub command " << cmd->name() << " already exist." << std::endl;
        exit(EXIT_FAILURE);
    }
    sub_commands_[cmd->name()] = cmd;
}

void ccmd::c_command::print_help() {
    std::cout << "command " << name() << ":" << std::endl;
    std::cout << "  " << help_long() << std::endl;
    std::cout << std::endl << "example: " << std::endl << "  " << example() << std::endl;
    std::cout << std::endl << "usage: " << std::endl << "  " << usage() << std::endl;
    std::cout << std::endl;
    if (sub_commands_.size()) {
        std::cout << "subcommands: " << std::endl;
        print_sub_command();
        std::cout << std::endl;
    }
    std::cout << "options: " << std::endl;
    print_flag_set();
}

void ccmd::c_command::print_sub_command() {
    for (auto &it : sub_commands_) {
        std::cout << "  " << it.second->name() << "\t" << it.second->help_short() << std::endl;
    }
}

void ccmd::c_command::print_flag_set() {
    flag_set_->print_flags();
}

void ccmd::c_command::parse_(std::vector<std::string> &arguments) {
    std::string cmd_name = arguments.at(0);
    check_help_(arguments);
    if (sub_commands_.size() == 0) {
        flag_set_->parse(arguments);
        run_(shared_from_this());
        return;
    }
    if (arguments.size() == 1 || (arguments.size() > 1 && arguments.at(1).substr(0, 1) == "-")) {
        flag_set_->parse(arguments);
        run_(shared_from_this());
        return;
    }

    cmd_name = arguments.at(1);
    auto it = sub_commands_.find(cmd_name); 
    if (it == sub_commands_.end()) {
        std::cerr << "command " << cmd_name << " not found." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::vector<std::string> next_arguments(arguments.begin() + 1, arguments.end());
    it->second->parse_(next_arguments);
}

void ccmd::c_command::check_help_(std::vector<std::string> &arguments) {
    if (arguments.size() < 2) {
        return;
    }

    std::string cmd_name = arguments.at(1);
    // print subcommand help info.
    if (cmd_name == k_help_command_name) {
        if (arguments.size() < 3) {
            std::cerr << "please input command name after help." << std::endl;
            exit(EXIT_FAILURE);
        }
        cmd_name = arguments.at(2);
        auto it = sub_commands_.find(cmd_name);
        if (it != sub_commands_.end()) {
            it->second->print_help();
            exit(EXIT_SUCCESS);
        }
        std::cerr << "command " << cmd_name << " not found." << std::endl;
        exit(EXIT_FAILURE);
    }

    // print command help info.
    if (cmd_name.substr(0, 1) == "-") {
        for (int i = 1; i < arguments.size(); i++) {
            if (arguments.at(i) == k_help_flag_long_name ||
                    arguments.at(i) == k_help_flag_short_name) {
                print_help();
                exit(EXIT_SUCCESS);
            }
        }
    }
}
