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
#include "Raychel_assert.h"

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
            //-h or --help should not mutate any state and exit immediately
            if (handle_help_arg(argc, argv)) {
                return false;
            }

            for (int i = 1; i < argc - 1; i += 2) {      //the first and last command line argument cannot be keys
                std::string_view current_arg = argv[i];  //NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                std::string_view next_arg = argv[i + 1]; //NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

                if (!current_arg.starts_with('-')) {
                    Logger::error("Argument '", current_arg, "' is incorrectly formatted!\n");
                    goto parse_failed; //NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto): Code duplication is worse than goto
                }
                if (current_arg.starts_with("--")) {
                    //parse argument in the form --key value
                    current_arg.remove_prefix(2U);
                    if (!_parse_long_arg(current_arg, next_arg)) {
                        goto parse_failed; //NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto): Code duplication is worse than goto
                    }
                } else {
                    //parse argument in the form -key value
                    current_arg.remove_prefix(1U);
                    if (!_parse_short_arg(current_arg, next_arg)) {
                        goto parse_failed; //NOLINT(hicpp-avoid-goto, cppcoreguidelines-avoid-goto): Code duplication is worse than goto
                    }
                }
            }
            return true;

        parse_failed:
            _show_usage();
            return false;
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

        [[nodiscard]] static bool _populate_value_ref(std::string_view value_str, const CommandLineValueReference& value) noexcept
        {
            using Type = CommandLineValueReference::ValueType;

            switch (value.type()) {
                case Type::int_:
                    return _parse_int(value_str, value);
                case Type::float_:
                    return _parse_float(value_str, value);
                case Type::string_:
                    value.as_string_ref() = value_str;
                    return true;
            }
            RAYCHEL_ASSERT_NOT_REACHED;
        }

        [[nodiscard]] static bool _parse_int(std::string_view value_str, const CommandLineValueReference& value) noexcept
        {
            const auto res = std::from_chars(std::begin(value_str), std::end(value_str), value.as_int_ref());

            if (res.ec != std::errc{}) {
                Logger::error("Could not parse value '", value_str, "' as an int!\n");
                return false;
            }
            return true;
        }

        [[nodiscard]] static bool _parse_float(std::string_view value_str, const CommandLineValueReference& value) noexcept
        {
            if constexpr (details::std_has_float_from_chars_v) {
                const auto res = std::from_chars(std::begin(value_str), std::end(value_str), value.as_float_ref());

                if (res.ec != std::errc{}) {
                    Logger::error("Could not parse value '", value_str, "' as a float!\n");
                    return false;
                }
                return true;
            } else {
                std::stringstream interpreter;
                interpreter << value_str;
                float f = 0.0F;
                if (interpreter >> f) {
                    value.as_float_ref() = f;
                    return true;
                }
                Logger::error("Could not parse value '", value_str, "' as a float!\n");
                return false;
            }
        }

        [[nodiscard]] bool handle_help_arg(int argc, char const* const* argv) noexcept
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
