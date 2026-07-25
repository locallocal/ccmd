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

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <type_traits>

namespace ccmd {
    namespace detail {

        class flag_value_base {
        public:
            virtual ~flag_value_base() = default;
        };

        template<typename T>
        class flag_value : public flag_value_base {
        public:
            explicit flag_value(const T &initial_value) : value(initial_value) {}

            T value;
        };

        template<typename T>
        struct is_supported_flag : std::false_type {};

        template<>
        struct is_supported_flag<bool> : std::true_type {};

        template<>
        struct is_supported_flag<int> : std::true_type {};

        template<>
        struct is_supported_flag<float> : std::true_type {};

        template<>
        struct is_supported_flag<std::string> : std::true_type {};

        template<typename T>
        inline void bind_flag(cflag::c_flag_set &, T *, const std::string &, const std::string &, const T &,
                const std::string &) {
            static_assert(is_supported_flag<T>::value,
                "ccmd flags support bool, int, float, and std::string");
        }

        inline void bind_flag(cflag::c_flag_set &flag_set, bool *value, const std::string &name,
                const std::string &short_name, const bool &default_value, const std::string &usage) {
            std::string mutable_name = name;
            std::string mutable_short_name = short_name;
            std::string mutable_usage = usage;
            flag_set.bool_varp(
                value, mutable_name, mutable_short_name, default_value, mutable_usage);
        }

        inline void bind_flag(cflag::c_flag_set &flag_set, int *value, const std::string &name,
                const std::string &short_name, const int &default_value, const std::string &usage) {
            std::string mutable_name = name;
            std::string mutable_short_name = short_name;
            std::string mutable_usage = usage;
            flag_set.int_varp(
                value, mutable_name, mutable_short_name, default_value, mutable_usage);
        }

        inline void bind_flag(cflag::c_flag_set &flag_set, float *value, const std::string &name,
                const std::string &short_name, const float &default_value, const std::string &usage) {
            std::string mutable_name = name;
            std::string mutable_short_name = short_name;
            std::string mutable_usage = usage;
            flag_set.float_varp(
                value, mutable_name, mutable_short_name, default_value, mutable_usage);
        }

        inline void bind_flag(cflag::c_flag_set &flag_set, std::string *value, const std::string &name,
                const std::string &short_name, const std::string &default_value, const std::string &usage) {
            std::string mutable_name = name;
            std::string mutable_short_name = short_name;
            std::string mutable_default_value = default_value;
            std::string mutable_usage = usage;
            flag_set.string_varp(
                value, mutable_name, mutable_short_name, mutable_default_value, mutable_usage);
        }

    }

    template<typename T>
    inline T c_command::var(const std::string &name) const {
        static_assert(detail::is_supported_flag<T>::value,
            "ccmd flags support bool, int, float, and std::string");

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

    template<typename T>
    inline void c_command::var(
            const std::string &name, T default_value, const std::string &usage) {
        varp<T>(name, "", default_value, usage);
    }

    template<typename T>
    inline void c_command::varp(const std::string &name, const std::string &short_name,
            T default_value, const std::string &usage) {
        static_assert(detail::is_supported_flag<T>::value,
            "ccmd flags support bool, int, float, and std::string");

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

        detail::bind_flag(
            *flag_set_, &value->value, name, short_name, default_value, usage);
    }

}
