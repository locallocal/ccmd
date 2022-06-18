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

#include <string>
#include <memory>
#include <functional>
#include "cflag.h"

namespace ccmd {

    extern const std::string k_help_command_name;
    extern const std::string k_help_flag_long_name;
    extern const std::string k_help_flag_short_name;


    class c_command : public std::enable_shared_from_this<c_command> {
    public:
        c_command(const char *name, const char *example, const char *usage, const char *help_long, const char *help_short,
            std::function<void(std::shared_ptr<c_command>)> run);
        c_command(std::string &name, std::string &example, std::string &usage, std::string &help_long,
            std::string &help_short, std::function<void(std::shared_ptr<c_command>)> run);
        c_command(const c_command &cmd) = delete;
        c_command(const c_command &&cmd) = delete;
        c_command&operator=(const c_command &cmd) = delete;
        ~c_command() = default;
    
    public:
        std::vector<std::string> &args();
        void execute(int argc, char *argv[]);
        void execute(std::vector<std::string> &arguments);
        void add_subcommand(std::shared_ptr<c_command> cmd);
        void print_help();
        void print_sub_command();
        void print_flag_set();

        // bool flag
        bool bool_var(const char *name);
        bool bool_var(std::string &name);
        void bool_var(const char *name, bool default_value, const char *usage);
        void bool_varp(const char *name, const char *short_name, bool default_value, const char *usage);
        void bool_var(std::string &name, bool default_value, std::string &usage);
        void bool_varp(std::string &name, std::string &short_name, bool default_value, std::string &usage);
        // int flag
        int int_var(const char *name);
        int int_var(std::string &name);
        void int_var(const char *name, int default_value, const char *usage);
        void int_varp(const char *name, const char *short_name, int default_value, const char *usage);
        void int_var(std::string &name, int default_value, std::string &usage);
        void int_varp(std::string &name, std::string &short_name, int default_value, std::string &usage);
        // float flag
        float float_var(const char *name);
        float float_var(std::string &name);
        void float_var(const char *name, float default_value, const char *usage);
        void float_varp(const char *name, const char *short_name, float default_value, const char *usage);
        void float_var(std::string &name, float default_value, std::string &usage);
        void float_varp(std::string &name, std::string &short_name, float default_value, std::string &usage);
        // string flag
        std::string &string_var(const char *name);
        std::string &string_var(std::string &name);
        void string_var(const char *name, const char *default_value, const char *usage);
        void string_varp(const char *name, const char *short_name, const char *default_value, const char *usage);
        void string_var(std::string &name, std::string &default_value, std::string &usage);
        void string_varp(std::string &name, std::string &short_name, std::string &default_value, std::string &usage);

    private:
        void parse_(std::vector<std::string> &arguments);
        void check_help_(std::vector<std::string> &arguments);

    public:
        // setter
        void usage(std::string &usage) { usage_ = usage; }
        void example(std::string &example) { example_ = example; }
        void help_short(std::string &help_short) { help_short_ = help_short; }
        void help_long(std::string &help_long) { help_long_ = help_long; }

        // getter
        std::string &name() { return name_; }
        std::string &usage() { return usage_; }
        std::string &example() { return example_; }
        std::string &help_short() { return help_short_; }
        std::string &help_long() { return help_long_; }
        std::shared_ptr<cflag::c_flag_set> flag_set() { return flag_set_; }
        std::map<std::string, std::shared_ptr<c_command>> &sub_commands() { return sub_commands_; }

    private:
        std::string name_;
        std::string usage_;
        std::string example_;
        std::string help_short_;
        std::string help_long_;
        std::shared_ptr<cflag::c_flag_set> flag_set_;
        std::function<void(std::shared_ptr<c_command>)> run_;

        std::map<std::string, std::shared_ptr<c_command>> sub_commands_;
        std::map<std::string, std::shared_ptr<bool>> bool_vars_;
        std::map<std::string, std::shared_ptr<bool>> bool_short_vars_;
        std::map<std::string, std::shared_ptr<int>> int_vars_;
        std::map<std::string, std::shared_ptr<int>> int_short_vars_;
        std::map<std::string, std::shared_ptr<float>> float_vars_;
        std::map<std::string, std::shared_ptr<float>> float_short_vars_;
        std::map<std::string, std::shared_ptr<std::string>> string_vars_;
        std::map<std::string, std::shared_ptr<std::string>> string_short_vars_;
    }; // class c_command

} // namespace ccmd


