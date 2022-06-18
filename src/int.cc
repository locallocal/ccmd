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

int ccmd::c_command::int_var(const char *name) {
    std::string var_name = name;
    return int_var(var_name);
}

int ccmd::c_command::int_var(std::string &name) {
    auto it = int_vars_.find(name);
    if (it != int_vars_.end()) {
        return *(it->second);
    }
    it = int_short_vars_.find(name);
    if (it != int_short_vars_.end()) {
        return *(it->second);
    }

    std::cerr << this->name() << " flag " << name << " not found." << std::endl;
    exit(EXIT_FAILURE);
}

void ccmd::c_command::int_var(const char *name, int default_value, const char *usage) {
    std::string var_name = name;
    std::string var_usage = usage;
    int_var(var_name, default_value, var_usage);
}

void ccmd::c_command::int_varp(const char *name, const char *short_name, int default_value, const char *usage) {
    std::string var_name =name;
    std::string var_short_name = short_name;
    std::string var_usage = usage;
    int_varp(var_name, var_short_name, default_value, var_usage);
}

void ccmd::c_command::int_var(std::string &name, int default_value, std::string &usage) {
    std::shared_ptr<int> var = std::make_shared<int>();
    auto it = int_vars_.find(name);
    if (it != int_vars_.end()) {
        std::cerr << this->name() << " flag " << name << " already exist." << std::endl;
        exit(EXIT_FAILURE);
    }
    int_vars_[name] = var;
    flag_set_->int_var(var.get(), name, default_value, usage);
}

void ccmd::c_command::int_varp(std::string &name, std::string &short_name, int default_value, std::string &usage) {
    std::shared_ptr<int> var = std::make_shared<int>();
    if (name.size()) {
        auto it = int_vars_.find(name);
        if (it != int_vars_.end()) {
            std::cerr << this->name() << " flag " << name << " already exist." << std::endl;
            exit(EXIT_FAILURE);
        }
        int_vars_[name] = var;
    }

    if (short_name.size()) {
        auto it = int_short_vars_.find(short_name);
        if (it != int_short_vars_.end()) {
            std::cerr << this->name() << "flag " << short_name << " already exist." << std::endl;
            exit(EXIT_FAILURE);
        }
        int_short_vars_[short_name] = var;
    }
    flag_set_->int_varp(var.get(), name, short_name, default_value, usage);
}
