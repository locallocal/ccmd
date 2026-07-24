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

std::string &ccmd::c_command::string_var(const char *name) {
    const std::string var_name = name;
    return string_var(var_name);
}

std::string &ccmd::c_command::string_var(std::string &name) {
    return string_var(static_cast<const std::string &>(name));
}

std::string &ccmd::c_command::string_var(const std::string &name) {
    return const_cast<std::string &>(static_cast<const c_command &>(*this).string_var(name));
}

const std::string &ccmd::c_command::string_var(const std::string &name) const {
    auto it = string_vars_.find(name);
    if (it != string_vars_.end()) {
        return *(it->second);
    }
    it = string_short_vars_.find(name);
    if (it != string_short_vars_.end()) {
        return *(it->second);
    }

    std::cerr << this->name() << " flag " << name << " not found." << std::endl;
    exit(EXIT_FAILURE);
}

void ccmd::c_command::string_var(const char *name, const char *default_value, const char *usage) {
    std::string var_name = name;
    std::string var_usage = usage;
    std::string var_default_value = default_value;
    string_var(var_name, var_default_value, var_usage);
}

void ccmd::c_command::string_varp(const char *name, const char *short_name, const char *default_value,
        const char *usage) {
    std::string var_name = name;
    std::string var_short_name = short_name;
    std::string var_default_value = default_value;
    std::string var_usage = usage;
    string_varp(var_name, var_short_name, var_default_value, var_usage);
}

void ccmd::c_command::string_var(std::string &name, std::string &default_value, std::string &usage) {
    string_var(static_cast<const std::string &>(name), static_cast<const std::string &>(default_value),
        static_cast<const std::string &>(usage));
}

void ccmd::c_command::string_var(const std::string &name, const std::string &default_value,
        const std::string &usage) {
    std::shared_ptr<std::string> var = std::make_shared<std::string>();
    auto it = string_vars_.find(name);
    if (it != string_vars_.end()) {
        std::cerr << this->name() << " flag " << name << " already exist." << std::endl;
        exit(EXIT_FAILURE);
    }
    string_vars_[name] = var;
    std::string mutable_name = name;
    std::string mutable_default_value = default_value;
    std::string mutable_usage = usage;
    flag_set_->string_var(var.get(), mutable_name, mutable_default_value, mutable_usage);
    remember_flag_(name, "", "string", default_value, usage, true);
}

void ccmd::c_command::string_varp(std::string &name, std::string &short_name, std::string &default_value,
        std::string &usage) {
    string_varp(static_cast<const std::string &>(name), static_cast<const std::string &>(short_name),
        static_cast<const std::string &>(default_value), static_cast<const std::string &>(usage));
}

void ccmd::c_command::string_varp(const std::string &name, const std::string &short_name,
        const std::string &default_value, const std::string &usage) {
    std::shared_ptr<std::string> var = std::make_shared<std::string>();
    if (name.size()) {
        auto it = string_vars_.find(name);
        if (it != string_vars_.end()) {
            std::cerr << this->name() << " flag " << name << " already exist." << std::endl;
            exit(EXIT_FAILURE);
        }
        string_vars_[name] = var;
    }

    if (short_name.size()) {
        auto it = string_short_vars_.find(short_name);
        if (it != string_short_vars_.end()) {
            std::cerr << this->name() << " flag " << short_name << " already exist." << std::endl;
            exit(EXIT_FAILURE);
        }
        string_short_vars_[short_name] = var;
    }
    std::string mutable_name = name;
    std::string mutable_short_name = short_name;
    std::string mutable_default_value = default_value;
    std::string mutable_usage = usage;
    flag_set_->string_varp(
        var.get(), mutable_name, mutable_short_name, mutable_default_value, mutable_usage);
    remember_flag_(name, short_name, "string", default_value, usage, true);
}
