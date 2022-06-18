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

bool ccmd::c_command::bool_var(const char *name) {
    std::string flag_name = name;
    return bool_var(flag_name);
}

bool ccmd::c_command::bool_var(std::string &name) {
    auto it = bool_vars_.find(name);
    if (it != bool_vars_.end()) {
        return *(it->second);
    }
    it = bool_short_vars_.find(name);
    if (it != bool_short_vars_.end()) {
        return *(it->second);
    }
    
    std::cerr << this->name() << " flag " << name << " not found." << std::endl;
    exit(EXIT_FAILURE);
}

void ccmd::c_command::bool_var(const char *name, bool default_value, const char *usage) {
    std::string var_name = name;
    std::string var_usage = usage;
    bool_var(var_name, default_value, var_usage);
}

void ccmd::c_command::bool_varp(const char *name, const char *short_name, bool default_value,
        const char *usage) {
    std::string var_name = name;
    std::string var_short_name = short_name;
    std::string var_usage = usage;
    bool_varp(var_name, var_short_name, default_value, var_usage);
}

void ccmd::c_command::bool_var(std::string &name, bool default_value, std::string &usage) {
    auto it = bool_vars_.find(name);
    if (it != bool_vars_.end()) {
        std::cerr << this->name() << " flag " << name << " already exist." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::shared_ptr<bool> var= std::make_shared<bool>();
    bool_vars_[name] = var;
    flag_set_->bool_var(var.get(), name, default_value, usage);
}

void ccmd::c_command::bool_varp(std::string &name, std::string &short_name, bool default_value,
        std::string &usage) {
    std::shared_ptr<bool> var= std::make_shared<bool>();
    if (name.size()) {
        auto it = bool_vars_.find(name);
        if (it != bool_vars_.end()) {
            std::cerr << this->name() << " flag " << name << " already exist." << std::endl;
            exit(EXIT_FAILURE);
        }
        bool_vars_[name] = var;
    }

    if (short_name.size()) {
        auto it = bool_short_vars_.find(short_name);
        if (it != bool_short_vars_.end()) {
            std::cerr << this->name() << " flag " << short_name << " already exist." << std::endl;
            exit(EXIT_FAILURE);
        }
        bool_short_vars_[short_name] = var;
    }
    flag_set_->bool_varp(var.get(), name, short_name, default_value, usage);
}
