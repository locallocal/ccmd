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

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <utility>

const std::string ccmd::k_help_command_name = "help";
const std::string ccmd::k_help_flag_long_name = "--help";
const std::string ccmd::k_help_flag_short_name = "-h";

ccmd::c_command::c_command(const char *name, const char *example, const char *usage, const char *help_long,
        const char *help_short, run_callback run)
        : c_command(std::string(name), std::string(example), std::string(usage), std::string(help_long),
              std::string(help_short), std::move(run)) {}

ccmd::c_command::c_command(std::string &name, std::string &example, std::string &usage, std::string &help_long,
        std::string &help_short, run_callback run)
        : c_command(static_cast<const std::string &>(name), static_cast<const std::string &>(example),
              static_cast<const std::string &>(usage), static_cast<const std::string &>(help_long),
              static_cast<const std::string &>(help_short), std::move(run)) {}

ccmd::c_command::c_command(const std::string &name, const std::string &example, const std::string &usage,
        const std::string &help_long, const std::string &help_short, run_callback run)
        : name_(name),
          usage_(usage),
          example_(example),
          help_short_(help_short),
          help_long_(help_long),
          flag_set_(std::make_shared<cflag::c_flag_set>(name_)),
          run_(std::move(run)) {
    if (name_.empty()) {
        throw std::invalid_argument("command name must not be empty");
    }
}

std::vector<std::string> &ccmd::c_command::args() {
    return flag_set_->args();
}

const std::vector<std::string> &ccmd::c_command::args() const {
    return flag_set_->args();
}

void ccmd::c_command::execute(int argc, char *argv[]) {
    if (argc <= 0 || argv == nullptr) {
        throw std::invalid_argument("execute requires at least a program name");
    }

    std::vector<std::string> arguments;
    arguments.reserve(static_cast<std::size_t>(argc));
    for (int i = 0; i < argc; ++i) {
        if (argv[i] == nullptr) {
            throw std::invalid_argument("execute received a null argument");
        }
        arguments.push_back(argv[i]);
    }
    execute(arguments);
}

void ccmd::c_command::execute(std::vector<std::string> &arguments) {
    execute(static_cast<const std::vector<std::string> &>(arguments));
}

void ccmd::c_command::execute(const std::vector<std::string> &arguments) {
    if (arguments.empty()) {
        throw std::invalid_argument("execute requires at least a program name");
    }

    std::vector<std::string> mutable_arguments(arguments);
    parse_(mutable_arguments);
}

void ccmd::c_command::add_subcommand(std::shared_ptr<ccmd::c_command> cmd) {
    if (cmd == nullptr) {
        throw std::invalid_argument("subcommand must not be null");
    }

    auto it = sub_commands_.find(cmd->name());
    if (it != sub_commands_.end()) {
        std::cerr << "error: subcommand '" << cmd->name() << "' already exists." << std::endl;
        exit(EXIT_FAILURE);
    }
    sub_commands_[cmd->name()] = cmd;
}

void ccmd::c_command::print_help() {
    std::cout << name() << " - " << help_short() << std::endl;
    if (!help_long().empty() && help_long() != help_short()) {
        std::cout << std::endl << help_long() << std::endl;
    }

    std::cout << std::endl << "Usage:" << std::endl << "  " << usage() << std::endl;
    if (!example().empty()) {
        std::cout << std::endl << "Example:" << std::endl << "  " << example() << std::endl;
    }

    if (!sub_commands_.empty()) {
        std::cout << std::endl << "Commands:" << std::endl;
        print_sub_command();
    }

    std::cout << std::endl << "Options:" << std::endl;
    print_flag_set();
}

void ccmd::c_command::print_sub_command() {
    std::size_t width = 0;
    for (const auto &entry : sub_commands_) {
        width = std::max(width, entry.second->name().size());
    }
    for (const auto &entry : sub_commands_) {
        std::cout << "  " << std::left << std::setw(static_cast<int>(width + 2)) << entry.second->name()
                  << entry.second->help_short() << std::endl;
    }
}

void ccmd::c_command::print_flag_set() {
    std::vector<std::pair<std::string, std::string>> rows;
    rows.emplace_back("-h, --help", "Show help information.");

    for (const auto &flag : flag_descriptions_) {
        std::string label = flag.short_name.empty() ? "    " : "-" + flag.short_name + ", ";
        if (!flag.name.empty()) {
            label += "--" + flag.name;
        }
        if (flag.type == "bool") {
            label += "[=<bool>]";
        } else {
            label += " <" + flag.type + ">";
        }

        std::string description = flag.usage;
        if (!description.empty() && description.back() != '.' && description.back() != '!' &&
                description.back() != '?') {
            description += '.';
        }
        description += " (default: ";
        if (flag.quote_default) {
            description += "\"" + flag.default_value + "\"";
        } else {
            description += flag.default_value;
        }
        description += ")";
        rows.emplace_back(std::move(label), std::move(description));
    }

    std::size_t width = 0;
    for (const auto &row : rows) {
        width = std::max(width, row.first.size());
    }
    for (const auto &row : rows) {
        std::cout << "  " << std::left << std::setw(static_cast<int>(width + 2)) << row.first << row.second
                  << std::endl;
    }
}

void ccmd::c_command::parse_(std::vector<std::string> &arguments) {
    if (arguments.empty()) {
        throw std::invalid_argument("execute requires at least a program name");
    }

    flag_set_->args().clear();
    check_help_(arguments);
    if (sub_commands_.empty()) {
        flag_set_->parse(arguments);
        if (run_) {
            run_(shared_from_this());
        }
        return;
    }
    if (arguments.size() == 1 || arguments.at(1).compare(0, 1, "-") == 0) {
        flag_set_->parse(arguments);
        if (run_) {
            run_(shared_from_this());
        }
        return;
    }

    const std::string &cmd_name = arguments.at(1);
    auto it = sub_commands_.find(cmd_name);
    if (it == sub_commands_.end()) {
        std::cerr << "error: command '" << cmd_name << "' not found for '" << name() << "'." << std::endl;
        std::cerr << "Run '" << arguments.front() << " help' to see available commands." << std::endl;
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
    if (cmd_name == k_help_command_name) {
        if (arguments.size() < 3) {
            print_help();
            exit(EXIT_SUCCESS);
        }

        cmd_name = arguments.at(2);
        auto it = sub_commands_.find(cmd_name);
        if (it != sub_commands_.end()) {
            if (arguments.size() > 3) {
                std::vector<std::string> next_arguments{cmd_name, k_help_command_name};
                next_arguments.insert(next_arguments.end(), arguments.begin() + 3, arguments.end());
                it->second->check_help_(next_arguments);
            }
            it->second->print_help();
            exit(EXIT_SUCCESS);
        }
        std::cerr << "error: command '" << cmd_name << "' not found for '" << name() << "'." << std::endl;
        std::cerr << "Run '" << arguments.front() << " help' to see available commands." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (sub_commands_.empty() || cmd_name.compare(0, 1, "-") == 0) {
        for (std::size_t i = 1; i < arguments.size(); ++i) {
            if (arguments.at(i) == "--") {
                break;
            }
            if (arguments.at(i) == k_help_flag_long_name ||
                    arguments.at(i) == k_help_flag_short_name) {
                print_help();
                exit(EXIT_SUCCESS);
            }
        }
    }
}

void ccmd::c_command::remember_flag_(const std::string &name, const std::string &short_name,
        const std::string &type, const std::string &default_value, const std::string &usage, bool quote_default) {
    flag_descriptions_.push_back({name, short_name, type, default_value, usage, quote_default});
}
