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

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "cflag.h"

namespace ccmd {

    namespace detail {
        class flag_value_base;
    }

    class c_command : public std::enable_shared_from_this<c_command> {
    public:
        using run_callback = std::function<void(std::shared_ptr<c_command>)>;

        c_command(const std::string &name, const std::string &example, const std::string &usage,
            const std::string &help_long, const std::string &help_short, run_callback run = run_callback());
        c_command(const c_command &cmd) = delete;
        c_command(c_command &&cmd) = delete;
        c_command &operator=(const c_command &cmd) = delete;
        c_command &operator=(c_command &&cmd) = delete;
        ~c_command() = default;

    public:
        std::vector<std::string> &args();
        const std::vector<std::string> &args() const;
        void execute(int argc, char *argv[]);
        void execute(const std::vector<std::string> &arguments);
        void add_subcommand(std::shared_ptr<c_command> cmd);
        void print_help();
        void print_sub_command();
        void print_flag_set();

        template<typename T>
        T var(const std::string &name) const;

        template<typename T>
        void var(const std::string &name, T default_value, const std::string &usage);

        template<typename T>
        void varp(const std::string &name, const std::string &short_name, T default_value,
            const std::string &usage);

    private:
        void parse_(std::vector<std::string> &arguments);
        void check_help_(std::vector<std::string> &arguments);

    public:
        // setter
        void usage(const std::string &usage) { usage_ = usage; }
        void example(const std::string &example) { example_ = example; }
        void help_short(const std::string &help_short) { help_short_ = help_short; }
        void help_long(const std::string &help_long) { help_long_ = help_long; }

        // getter
        std::string &name() { return name_; }
        const std::string &name() const { return name_; }
        std::string &usage() { return usage_; }
        const std::string &usage() const { return usage_; }
        std::string &example() { return example_; }
        const std::string &example() const { return example_; }
        std::string &help_short() { return help_short_; }
        const std::string &help_short() const { return help_short_; }
        std::string &help_long() { return help_long_; }
        const std::string &help_long() const { return help_long_; }
        std::shared_ptr<cflag::c_flag_set> flag_set() { return flag_set_; }
        std::shared_ptr<const cflag::c_flag_set> flag_set() const { return flag_set_; }
        std::map<std::string, std::shared_ptr<c_command>> &sub_commands() { return sub_commands_; }
        const std::map<std::string, std::shared_ptr<c_command>> &sub_commands() const { return sub_commands_; }

    private:
        std::string name_;
        std::string usage_;
        std::string example_;
        std::string help_short_;
        std::string help_long_;
        std::shared_ptr<cflag::c_flag_set> flag_set_;
        run_callback run_;

        std::map<std::string, std::shared_ptr<c_command>> sub_commands_;
        std::map<std::string, std::shared_ptr<detail::flag_value_base>> flag_values_;
    };

}

#include "ccmd/detail/command.h"
#include "ccmd/detail/flag.h"
