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

    extern const std::string k_help_command_name;
    extern const std::string k_help_flag_long_name;
    extern const std::string k_help_flag_short_name;

    class c_command : public std::enable_shared_from_this<c_command> {
    public:
        using run_callback = std::function<void(std::shared_ptr<c_command>)>;

        c_command(const char *name, const char *example, const char *usage, const char *help_long, const char *help_short,
            run_callback run = run_callback());
        c_command(std::string &name, std::string &example, std::string &usage, std::string &help_long,
            std::string &help_short, run_callback run);
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
        void execute(std::vector<std::string> &arguments);
        void execute(const std::vector<std::string> &arguments);
        void add_subcommand(std::shared_ptr<c_command> cmd);
        void print_help();
        void print_sub_command();
        void print_flag_set();

        // bool flag
        bool bool_var(const char *name);
        bool bool_var(std::string &name);
        bool bool_var(const std::string &name);
        bool bool_var(const std::string &name) const;
        void bool_var(const char *name, bool default_value, const char *usage);
        void bool_varp(const char *name, const char *short_name, bool default_value, const char *usage);
        void bool_var(std::string &name, bool default_value, std::string &usage);
        void bool_varp(std::string &name, std::string &short_name, bool default_value, std::string &usage);
        void bool_var(const std::string &name, bool default_value, const std::string &usage);
        void bool_varp(const std::string &name, const std::string &short_name, bool default_value,
            const std::string &usage);
        // int flag
        int int_var(const char *name);
        int int_var(std::string &name);
        int int_var(const std::string &name);
        int int_var(const std::string &name) const;
        void int_var(const char *name, int default_value, const char *usage);
        void int_varp(const char *name, const char *short_name, int default_value, const char *usage);
        void int_var(std::string &name, int default_value, std::string &usage);
        void int_varp(std::string &name, std::string &short_name, int default_value, std::string &usage);
        void int_var(const std::string &name, int default_value, const std::string &usage);
        void int_varp(const std::string &name, const std::string &short_name, int default_value,
            const std::string &usage);
        // float flag
        float float_var(const char *name);
        float float_var(std::string &name);
        float float_var(const std::string &name);
        float float_var(const std::string &name) const;
        void float_var(const char *name, float default_value, const char *usage);
        void float_varp(const char *name, const char *short_name, float default_value, const char *usage);
        void float_var(std::string &name, float default_value, std::string &usage);
        void float_varp(std::string &name, std::string &short_name, float default_value, std::string &usage);
        void float_var(const std::string &name, float default_value, const std::string &usage);
        void float_varp(const std::string &name, const std::string &short_name, float default_value,
            const std::string &usage);
        // string flag
        std::string &string_var(const char *name);
        std::string &string_var(std::string &name);
        std::string &string_var(const std::string &name);
        const std::string &string_var(const std::string &name) const;
        void string_var(const char *name, const char *default_value, const char *usage);
        void string_varp(const char *name, const char *short_name, const char *default_value, const char *usage);
        void string_var(std::string &name, std::string &default_value, std::string &usage);
        void string_varp(std::string &name, std::string &short_name, std::string &default_value, std::string &usage);
        void string_var(const std::string &name, const std::string &default_value, const std::string &usage);
        void string_varp(const std::string &name, const std::string &short_name, const std::string &default_value,
            const std::string &usage);

    private:
        void parse_(std::vector<std::string> &arguments);
        void check_help_(std::vector<std::string> &arguments);
        void remember_flag_(const std::string &name, const std::string &short_name, const std::string &type,
            const std::string &default_value, const std::string &usage, bool quote_default = false);

    public:
        // setter
        void usage(std::string &usage) { usage_ = usage; }
        void usage(const std::string &usage) { usage_ = usage; }
        void example(std::string &example) { example_ = example; }
        void example(const std::string &example) { example_ = example; }
        void help_short(std::string &help_short) { help_short_ = help_short; }
        void help_short(const std::string &help_short) { help_short_ = help_short; }
        void help_long(std::string &help_long) { help_long_ = help_long; }
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
        struct flag_description {
            std::string name;
            std::string short_name;
            std::string type;
            std::string default_value;
            std::string usage;
            bool quote_default;
        };

        std::string name_;
        std::string usage_;
        std::string example_;
        std::string help_short_;
        std::string help_long_;
        std::shared_ptr<cflag::c_flag_set> flag_set_;
        run_callback run_;

        std::map<std::string, std::shared_ptr<c_command>> sub_commands_;
        std::vector<flag_description> flag_descriptions_;
        std::map<std::string, std::shared_ptr<bool>> bool_vars_;
        std::map<std::string, std::shared_ptr<bool>> bool_short_vars_;
        std::map<std::string, std::shared_ptr<int>> int_vars_;
        std::map<std::string, std::shared_ptr<int>> int_short_vars_;
        std::map<std::string, std::shared_ptr<float>> float_vars_;
        std::map<std::string, std::shared_ptr<float>> float_short_vars_;
        std::map<std::string, std::shared_ptr<std::string>> string_vars_;
        std::map<std::string, std::shared_ptr<std::string>> string_short_vars_;
    };

}
