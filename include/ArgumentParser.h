/**
* \file ArgumentParser.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief header file for ArgumentParser class
* \date 2021-08-31
* 
* MIT License
* Copyright (c) [2021] [Weckyy702 (weckyy702@gmail.com | https://github.com/Weckyy702)]
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* 
*/
#ifndef RAYCHEL_CORE_ARGUMENT_PARSER_H
#define RAYCHEL_CORE_ARGUMENT_PARSER_H

#include <map>
#include <string>
#include <string_view>
#include <charconv>
#include "CommandLineValueReference.h"
#include "RaychelLogger/Logger.h"
#include "Raychel_assert.h"

namespace Raychel {

    bool case_insensitive_equal(std::string_view lhs, std::string_view rhs) noexcept
    {
        if(lhs.size() != rhs.size()) {
            return false;
        }
        if(lhs.data() == rhs.data()) {
            return true;
        }

        bool equal = true;
        for(std::size_t i = 0; i < lhs.size(); i++) {
            equal &= (std::tolower(lhs[i]) == std::tolower(rhs[i]));
        }

        return equal;
    }

    class ArgumentParser
    {
    public:
        ArgumentParser() = default;

        bool add_bool_arg(std::string_view name, bool& value_ref) noexcept
        {
            return _add_arg(name, value_ref);
        }

        bool add_int_arg(std::string_view name, int& value_ref) noexcept
        {
            return _add_arg(name, value_ref);
        }

        bool add_float_arg(std::string_view name, float& value_ref) noexcept
        {
            return _add_arg(name, value_ref);
        }

        bool add_string_arg(std::string_view name, std::string& value_ref) noexcept
        {
            return _add_arg(name, value_ref);
        }



        [[nodiscard]] bool parse(int argc, char** argv) noexcept
        {
            //TODO: clean this up and implement error handling [ have fun ;) ]
            using ValueType = CommandLineValueReference::ValueType;
            (void)this;
            bool should_exit = false;

            for(int i = 0; i < argc-1; i++) { //the last command line argument cannot be a key
                std::string_view current_arg = argv[i];

                if(!current_arg.starts_with("--")) {
                    continue;
                }
                current_arg.remove_prefix(2U);

                for(const auto&[key, value] : arguments_) {
                    if(key == current_arg) {
                        Logger::debug("Found key ", key, '\n');
                        std::string_view value_str = argv[i+1];
                        switch(value.type()) {
                            case ValueType::bool_:
                                value.as_bool() = case_insensitive_equal(value_str, "true");
                                break;
                            case ValueType::int_:
                                value.as_int() = std::atoi(value_str.data());
                                break;
                            case ValueType::float_:
                                std::from_chars(std::begin(value_str), std::end(value_str), value.as_float());
                                break;
                            case ValueType::string_:
                                value.as_string() = value_str;
                                break;
                        }
                        break;
                    }
                }
            }

            return !should_exit;
        }

    private:
        template <typename T>
        [[nodiscard]] bool _add_arg(std::string_view name_, T& value_ref) noexcept
        {
            const auto name = std::string{name_};
            if (arguments_.contains(name)) {
                return false;
            }

            return arguments_.insert({name, CommandLineValueReference{value_ref}}).second;
        }

        std::map<std::string, CommandLineValueReference> arguments_;
    };

} // namespace Raychel

#endif //!RAYCHEL_CORE_ARGUMENT_PARSER_H