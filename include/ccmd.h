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

#pragma once

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "cflag.h"

namespace ccmd {

namespace detail {

class flag_value_base {
public:
    virtual ~flag_value_base() = default;
};

// Owns the storage that cflag writes parsed values into. Every type accepted
// by cflag::flag_traits<T> (built-in or user specialized) is accepted here.
template <typename T>
class flag_value : public flag_value_base {
public:
    explicit flag_value(const T& initial_value) : value(initial_value) {}

    T value;
};

}  // namespace detail

class command : public std::enable_shared_from_this<command> {
public:
    using run_callback = std::function<void(std::shared_ptr<command>)>;

    command(const std::string& name, const std::string& example, const std::string& usage, const std::string& help_long,
            const std::string& help_short, run_callback run = run_callback());
    command(const command& cmd) = delete;
    command(command&& cmd) = delete;
    command& operator=(const command& cmd) = delete;
    command& operator=(command&& cmd) = delete;
    ~command() = default;

public:
    std::vector<std::string>& args();
    const std::vector<std::string>& args() const;
    void execute(int argc, char* argv[]);
    void execute(const std::vector<std::string>& arguments);
    void add_subcommand(std::shared_ptr<command> cmd);
    void print_help();
    void print_sub_command();
    void print_flag_set();

    template <typename T>
    T var(const std::string& name) const;

    template <typename T>
    void var(const std::string& name, T default_value, const std::string& usage);

    template <typename T>
    void varp(const std::string& name, const std::string& short_name, T default_value, const std::string& usage);

private:
    void parse_(std::vector<std::string>& arguments);
    void check_help_(std::vector<std::string>& arguments);

public:
    // setter
    void usage(const std::string& usage) { usage_ = usage; }
    void example(const std::string& example) { example_ = example; }
    void help_short(const std::string& help_short) { help_short_ = help_short; }
    void help_long(const std::string& help_long) { help_long_ = help_long; }

    // getter
    std::string& name() { return name_; }
    const std::string& name() const { return name_; }
    std::string& usage() { return usage_; }
    const std::string& usage() const { return usage_; }
    std::string& example() { return example_; }
    const std::string& example() const { return example_; }
    std::string& help_short() { return help_short_; }
    const std::string& help_short() const { return help_short_; }
    std::string& help_long() { return help_long_; }
    const std::string& help_long() const { return help_long_; }
    std::shared_ptr<cflag::flag_set> flag_set() { return flag_set_; }
    std::shared_ptr<const cflag::flag_set> flag_set() const { return flag_set_; }
    std::map<std::string, std::shared_ptr<command>>& sub_commands() { return sub_commands_; }
    const std::map<std::string, std::shared_ptr<command>>& sub_commands() const { return sub_commands_; }

private:
    std::string name_;
    std::string usage_;
    std::string example_;
    std::string help_short_;
    std::string help_long_;
    std::shared_ptr<cflag::flag_set> flag_set_;
    run_callback run_;

    std::map<std::string, std::shared_ptr<command>> sub_commands_;
    std::map<std::string, std::shared_ptr<detail::flag_value_base>> flag_values_;
};

// ---------------------------------------------------------------------------
// command implementation
// ---------------------------------------------------------------------------

inline command::command(const std::string& name, const std::string& example, const std::string& usage,
                        const std::string& help_long, const std::string& help_short, run_callback run)
    : name_(name),
      usage_(usage),
      example_(example),
      help_short_(help_short),
      help_long_(help_long),
      flag_set_(std::make_shared<cflag::flag_set>()),
      run_(std::move(run)) {
    if (name_.empty()) {
        throw std::invalid_argument("command name must not be empty");
    }
}

inline std::vector<std::string>& command::args() { return flag_set_->args(); }

inline const std::vector<std::string>& command::args() const { return flag_set_->args(); }

inline void command::execute(int argc, char* argv[]) {
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

inline void command::execute(const std::vector<std::string>& arguments) {
    if (arguments.empty()) {
        throw std::invalid_argument("execute requires at least a program name");
    }

    std::vector<std::string> mutable_arguments(arguments);
    parse_(mutable_arguments);
}

inline void command::add_subcommand(std::shared_ptr<command> cmd) {
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

inline void command::print_help() {
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

inline void command::print_sub_command() {
    std::size_t width = 0;
    for (const auto& entry : sub_commands_) {
        width = std::max(width, entry.second->name().size());
    }
    for (const auto& entry : sub_commands_) {
        std::cout << "  " << std::left << std::setw(static_cast<int>(width + 2)) << entry.second->name()
                  << entry.second->help_short() << std::endl;
    }
}

inline void command::print_flag_set() {
    // Mirrors the " -x  --name[type] usage(default)" rows that cflag prints.
    // help/-h are reserved in cflag and handled by ccmd, so they are listed here.
    std::cout << " -h  --help[bool] show help information.(false)" << std::endl;
    flag_set_->print_flags();
}

inline void command::parse_(std::vector<std::string>& arguments) {
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

    const std::string& cmd_name = arguments.at(1);
    auto it = sub_commands_.find(cmd_name);
    if (it == sub_commands_.end()) {
        std::cerr << "error: command '" << cmd_name << "' not found for '" << name() << "'." << std::endl;
        std::cerr << "Run '" << arguments.front() << " help' to see available commands." << std::endl;
        exit(EXIT_FAILURE);
    }
    std::vector<std::string> next_arguments(arguments.begin() + 1, arguments.end());
    it->second->parse_(next_arguments);
}

inline void command::check_help_(std::vector<std::string>& arguments) {
    if (arguments.size() < 2) {
        return;
    }

    std::string cmd_name = arguments.at(1);
    if (cmd_name == "help") {
        if (arguments.size() < 3) {
            print_help();
            exit(EXIT_SUCCESS);
        }

        cmd_name = arguments.at(2);
        auto it = sub_commands_.find(cmd_name);
        if (it != sub_commands_.end()) {
            if (arguments.size() > 3) {
                std::vector<std::string> next_arguments{cmd_name, "help"};
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
            if (arguments.at(i) == "--help" || arguments.at(i) == "-h") {
                print_help();
                exit(EXIT_SUCCESS);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// flag registration and lookup
// ---------------------------------------------------------------------------

template <typename T>
inline T command::var(const std::string& name) const {
    auto it = flag_values_.find(name);
    if (it == flag_values_.end()) {
        std::cerr << this->name() << " flag " << name << " not found." << std::endl;
        exit(EXIT_FAILURE);
    }

    auto value = std::dynamic_pointer_cast<detail::flag_value<T>>(it->second);
    if (value == nullptr) {
        std::cerr << this->name() << " flag " << name << " has a different type." << std::endl;
        exit(EXIT_FAILURE);
    }
    return value->value;
}

template <typename T>
inline void command::var(const std::string& name, T default_value, const std::string& usage) {
    varp<T>(name, "", default_value, usage);
}

template <typename T>
inline void command::varp(const std::string& name, const std::string& short_name, T default_value,
                          const std::string& usage) {
    if (name.empty() && short_name.empty()) {
        throw std::invalid_argument("flag name and short name must not both be empty");
    }

    auto value = std::make_shared<detail::flag_value<T>>(default_value);
    if (!name.empty()) {
        auto result = flag_values_.emplace(name, value);
        if (!result.second) {
            std::cerr << this->name() << " flag " << name << " already exist." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    if (!short_name.empty() && short_name != name) {
        auto result = flag_values_.emplace(short_name, value);
        if (!result.second) {
            std::cerr << this->name() << " flag " << short_name << " already exist." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    // cflag validates the names (reserved, duplicate, short-name length),
    // formats the default for --help and converts parsed text via flag_traits<T>.
    flag_set_->varp<T>(&value->value, name, short_name, default_value, usage);
}

}  // namespace ccmd
