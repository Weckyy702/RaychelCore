/**
* \file CommandLineHelper.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header for helper classes and functions for ArgumentParser class
* \date 2021-09-20
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
#ifndef RAYCHEL_COMMAND_LINE_HELPER_H
#define RAYCHEL_COMMAND_LINE_HELPER_H

#include "compat.h"

#include <charconv>
#include <string>
#if RAYCHEL_HAS_SPACESHIP_OP
    #include <compare>
#endif

namespace Raychel {

    struct CommandLineKey
    {
        std::string long_name;
        std::string short_name;
        std::string description;

#if RAYCHEL_HAS_SPACESHIP_OP
        auto operator<=>(const CommandLineKey& rhs) const noexcept
        {
            //Two CommandLineKeys can neither have the same long nor short name
            if ((long_name == rhs.long_name) || (short_name == rhs.short_name)) {
                return std::strong_ordering::equivalent;
            }
            return long_name <=> rhs.long_name;
        }
#else
        bool operator<(const CommandLineKey& rhs) const noexcept
        {
            if ((long_name == rhs.long_name) || (short_name == rhs.short_name)) {
                return false;
            }
            return long_name < rhs.long_name; //NOLINT: what?
        }
#endif
    };

    namespace details {

        template <typename = void>
        struct _std_has_float_from_chars : std::false_type
        {};

        template <>
        struct _std_has_float_from_chars<std::void_t<decltype(std::from_chars(
            std::declval<const char*>(), std::declval<const char*>(), std::declval<float&>()))>> : std::true_type
        {};

        constexpr auto std_has_float_from_chars_v = _std_has_float_from_chars<>::value;
    } // namespace details

} // namespace Raychel

#endif //!RAYCHEL_COMMAND_LINE_HELPER_H