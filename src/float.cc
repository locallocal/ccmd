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

float ccmd::c_command::float_var(const char *name) {
    std::string var_name = name;
    return float_var(var_name);
}

float ccmd::c_command::float_var(std::string &name) {
    auto it = float_vars_.find(name);
    if (it != float_vars_.end()) {
        return *(it->second);
    }
    it = float_short_vars_.find(name);
    if (it != float_short_vars_.end()) {
        return *(it->second);
    }

    std::cerr << this->name() << " flag " << name << " not found." << std::endl;
    exit(EXIT_FAILURE);
}

void ccmd::c_command::float_var(const char *name, float default_value, const char *usage) {
    std::string var_name = name;
    std::string var_usage = usage;
    float_var(var_name, default_value, var_usage);
}

void ccmd::c_command::float_varp(const char *name, const char *short_name, float default_value, const char *usage) {
    std::string var_name = name;
    std::string var_short_name = short_name;
    std::string var_usage = usage;
    float_varp(var_name, var_short_name, default_value, var_usage);
}

void ccmd::c_command::float_var(std::string &name, float default_value, std::string &usage) {
    auto it = float_vars_.find(name);
    if (it != float_vars_.end()) {
        std::cerr << this->name() << " flag " << name << " already exist." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::shared_ptr<float> var = std::make_shared<float>();
    float_vars_[name] = var;
    flag_set_->float_var(var.get(), name, default_value, usage);
}

void ccmd::c_command::float_varp(std::string &name, std::string &short_name, float default_value, std::string &usage) {
    std::shared_ptr<float> var = std::make_shared<float>();
    if (name.size()) {
        auto it = float_vars_.find(name);
        if (it != float_vars_.end()) {
            std::cerr << this->name() << " flag " << name << " already exist." << std::endl;
            exit(EXIT_FAILURE);
        }
        float_vars_[name] = var;
    }

    if (short_name.size()) {
        auto it = float_short_vars_.find(short_name);
        if (it != float_short_vars_.end()) {
            std::cerr << this->name() << " flag " << short_name << " already exist." << std::endl;
            exit(EXIT_FAILURE);
        }
        float_short_vars_[short_name] = var;
    }
    flag_set_->float_varp(var.get(), name, short_name, default_value, usage);
}
