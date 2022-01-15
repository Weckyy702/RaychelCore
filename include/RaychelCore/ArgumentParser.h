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

#include <charconv>
#include <map>
#include <optional>
#include <sstream>
#include <string_view>

#include "CommandLineHelper.h"
#include "CommandLineValueReference.h"
#include "RaychelLogger/Logger.h"

namespace Raychel {

    class ArgumentParser
    {

        using ArgumentMap_t = std::map<CommandLineKey, CommandLineValueReference>;

    public:
        ArgumentParser() = default;

        /**
        * \brief add an argument of type int to the parser
        * 
        * \param name name for the argument. Will match --name
        * \param short_name short name for the argument. Will match -short_name
        * \param description description for the argument. Used when printing Usage
        * \param value_ref reference for the argument. Will not be mutated if parsing fails
        * \return wether the argument could be added succesfully (arguments must be unique)
        */
        bool
        add_int_arg(std::string_view name, std::string_view short_name, std::string_view description, int& value_ref) noexcept
        {
            return _add_arg(name, short_name, description, value_ref);
        }

        /**
        * \brief add an argument of type float to the parser
        * 
        * \param name name for the argument. Will match --name
        * \param short_name short name for the argument. Will match -short_name
        * \param description description for the argument. Used when printing Usage
        * \param value_ref reference for the argument. Will not be mutated if parsing fails
        * \return wether the argument could be added succesfully (arguments must be unique)
        */
        bool
        add_float_arg(std::string_view name, std::string_view short_name, std::string_view description, float& value_ref) noexcept
        {
            return _add_arg(name, short_name, description, value_ref);
        }

        /**
        * \brief add an argument of type string to the parser
        * 
        * \param name name for the argument. Will match --name
        * \param short_name short name for the argument. Will match -short_name
        * \param description description for the argument. Used when printing Usage
        * \param value_ref reference for the argument. Will not be mutated if parsing fails
        * \return wether the argument could be added succesfully (arguments must be unique)
        */
        bool add_string_arg(
            std::string_view name, std::string_view short_name, std::string_view description, std::string& value_ref) noexcept
        {
            return _add_arg(name, short_name, description, value_ref);
        }

        /**
        * \brief Parse the command line arguments and print usage string if parsing fails
        * 
        * \param argc argc that was given to main()
        * \param argv argv that was given to main()
        * \return wether parsing was successful (most programs exit if parsing fails)
        */
        [[nodiscard]] bool parse(int argc, char const* const* argv) noexcept
        {
            // -h or --help should not mutate any state and exit immediately
            if (_handle_help_arg(argc, argv)) {
                return false;
            }

            if (!_parse(argc, argv)) {
                _show_usage();
                return false;
            }

            return true;
        }

    private:
        template <typename T>
        [[nodiscard]] bool
        _add_arg(std::string_view name_, std::string_view short_name_, std::string_view description_, T& value_ref) noexcept
        {
            const CommandLineKey key{std::string{name_}, std::string{short_name_}, std::string{description_}};
            if (arguments_.contains(key)) {
                return false;
            }

            return arguments_.insert({key, CommandLineValueReference{value_ref}}).second;
        }

        [[nodiscard]] bool _parse(int argc, char const* const* argv) noexcept
        {
            //parsing state
            bool got_key = false;
            std::string_view current_key;

            for (int i = 1; i < argc; i++) {   //the first argument cannot be a key
                std::string_view arg{argv[i]}; //NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

                if (got_key) {

                    if (!_parse_argument(current_key, arg)) {
                        return false;
                    }

                    got_key = false;
                } else {
                    if (!arg.starts_with('-')) {
                        Logger::error("Expected key starting with '-' or '--', got '", arg, "'\n");
                        return false;
                    }

                    //TODO: move this up into the parsing part of the loop to make the distinction better
                    if (const auto position = arg.find('='); position != std::string_view::npos) {
                        if (!_parse_assignment_option(arg, position)) {
                            return false;
                        }
                        got_key = false;
                        continue;
                    }

                    current_key = arg;
                    got_key = true;
                }
            }
            return true;
        }

        [[nodiscard]] bool _parse_argument(std::string_view key, std::string_view value_str) noexcept
        {
            if (key.starts_with("--")) {
                key.remove_prefix(2U);
                return _parse_long_arg(key, value_str);
            }
            key.remove_prefix(1U);
            return _parse_short_arg(key, value_str);
        }

        [[nodiscard]] bool _parse_assignment_option(std::string_view current_arg, std::size_t assignment_index) noexcept
        {
            const auto key = current_arg.substr(0, assignment_index);
            const auto value_str = current_arg.substr(assignment_index + 1);

            return _parse_argument(key, value_str);
        }

        [[nodiscard]] bool _parse_long_arg(std::string_view current_arg, std::string_view value_str) noexcept
        {
            const auto pair_opt = _find_value_reference_for_key(current_arg, true);
            if (!pair_opt) {
                Logger::error("Unknown option '", current_arg, "'!\n");
                return false;
            }
            return _populate_value_ref(value_str, pair_opt->second);
        };

        [[nodiscard]] bool _parse_short_arg(std::string_view current_arg, std::string_view value_str) noexcept
        {
            const auto pair_opt = _find_value_reference_for_key(current_arg, false);
            if (!pair_opt) {
                Logger::error("Unknown option '", current_arg, "'!\n");
                return false;
            }
            return _populate_value_ref(value_str, pair_opt->second);
        }

        [[nodiscard]] std::optional<ArgumentMap_t::value_type>
        _find_value_reference_for_key(std::string_view arg_key, bool long_arg) const noexcept
        {
            const auto _compare_key = [&arg_key, long_arg](const CommandLineKey& key) {
                if (long_arg) {
                    return key.long_name == arg_key;
                }
                return key.short_name == arg_key;
            };

            for (const auto& pair : arguments_) {
                if (_compare_key(pair.first)) {
                    return pair;
                }
            }
            return std::nullopt;
        }

        [[nodiscard]] static bool
        _populate_value_ref(std::string_view value_string, const CommandLineValueReference& value_ref) noexcept
        {
            using Type = CommandLineValueReference::ValueType;

            switch (value_ref.type()) {
                case Type::int_:
                    return _parse_int(value_string, value_ref);
                case Type::float_:
                    return _parse_float(value_string, value_ref);
                case Type::string_:
                    value_ref.as_string_ref() = value_string;
                    return true;
            }
            RAYCHEL_ASSERT_NOT_REACHED;
        }

        template <typename T = int>
        [[nodiscard]] static bool _parse_int(std::string_view value_string, const CommandLineValueReference& value_ref) noexcept
        {
            static_assert(std::is_same_v<T, int>, "Implementation bug!");
            if constexpr (details::_std_has_from_chars<T>::value) {
                const auto [_, ec] = std::from_chars(
                    value_string.data(), value_string.data() + value_string.size(), static_cast<T&>(value_ref.as_int_ref()));

                if (ec != std::errc{}) {
                    Logger::error("Could not parse value '", value_string, "' as an int!\n");
                    return false;
                }
                return true;
            } else {
                std::stringstream interpreter;
                interpreter << value_string;
                int f = 0;
                if (interpreter >> f) {
                    value_ref.as_int_ref() = f;
                    return true;
                }
                Logger::error("Could not parse value '", value_string, "' as an int!\n");
                return false;
            }
        }

        template <typename T = float>
        [[nodiscard]] static bool _parse_float(std::string_view value_string, const CommandLineValueReference& value_ref) noexcept
        {
            static_assert(std::is_same_v<T, float>, "Implementation bug!");
            if constexpr (details::_std_has_from_chars<T>::value) {
                const auto [_, ec] = std::from_chars(
                    value_string.data(), value_string.data() + value_string.size(), static_cast<T&>(value_ref.as_float_ref()));

                if (ec != std::errc{}) {
                    Logger::error("Could not parse value '", value_string, "' as a float!\n");
                    return false;
                }
                return true;
            } else {
                std::stringstream interpreter;
                interpreter << value_string;
                float f = 0.0F;
                if (interpreter >> f) {
                    value_ref.as_float_ref() = f;
                    return true;
                }
                Logger::error("Could not parse value '", value_string, "' as a float!\n");
                return false;
            }
        }

        [[nodiscard]] bool _handle_help_arg(int argc, char const* const* argv) noexcept
        {
            for (int i = 0; i < argc; i++) {
                std::string_view arg = argv[i]; //NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                if (arg == "-h" || arg == "--help") {
                    _show_usage();
                    return true;
                }
            }
            return false;
        }

        void _show_usage() noexcept
        {
            Logger::log("USAGE:\n\t--help -h\tsee this message and exit\n");
            for (const auto& [key, _] : arguments_) {
                Logger::log("\t--", key.long_name, " -", key.short_name, '\t', key.description, '\n');
            }
        }

        ArgumentMap_t arguments_;
    };

} // namespace Raychel

#endif //!RAYCHEL_CORE_ARGUMENT_PARSER_H
