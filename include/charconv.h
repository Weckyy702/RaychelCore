/**
* \file charconv.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for charconv function replacements
* \date 2021-11-09
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
#ifndef RAYCHELCORE_CHARCONV_H
#define RAYCHELCORE_CHARCONV_H

#include "./compat.h"

#if RAYCHEL_HAS_CHARCONV
    #include <charconv>
#else
    #pragma message(                                                                                                             \
        "IMPORTANT: You are using RaychelCore's replacement for std::from_chars. The integer version only supports the bases 8, 10 and 16!")
    #include <iomanip>
    #include <sstream>
    #include <string>
    #include <system_error>
#endif

namespace Raychel {

#if RAYCHEL_HAS_CHARCONV
    using std::chars_format;
    using std::from_chars_result;
    using std::to_chars_result;
#else

    enum class chars_format {
        scientific = 1,
        fixed = 2,
        hex = 4,
        general = fixed | scientific, //NOLINT(hicpp-signed-bitwise): we have to follow the standard exactly
    };

    struct from_chars_result
    {
        const char* ptr;
        std::errc ec;

    #if RAYCHEL_CPP_VERSION == RAYCHEL_CPP_VERSION_20
        bool operator==(const from_chars_result& other) const noexcept = default;
    #endif
    };

    struct to_chars_result
    {
        char* ptr;
        std::errc ec;
    };

#endif

#if RAYCHEL_HAS_CHARCONV
    using std::from_chars;
    using std::to_chars;
#else

    namespace details {
        template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
        from_chars_result
        fp_from_chars_impl(char const* const begin, char const* const end, T& value, std::ios::fmtflags format) noexcept
        {
            std::istringstream stream{std::string{begin, end}};
            stream.imbue(std::locale::classic());
            stream.setf(format);

            if (T val; (stream >> val)) {
                value = val;
                return {end, {}};
            }

            return {begin, std::errc::invalid_argument};
        }

        template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
        from_chars_result int_from_chars_impl(char const* const begin, char const* const end, T& value, int base) noexcept
        {
            if (base != 8 && base != 10 && base != 16) {
                return {begin, std::errc::invalid_argument};
            }

            std::istringstream stream{std::string{begin, end}};
            stream.imbue(std::locale::classic());
            stream >> std::setbase(base);

            if (T val; (stream >> val)) {
                value = val;
                return {end, {}};
            }

            return {begin, std::errc::invalid_argument};
        }
    } // namespace details

    template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    from_chars_result from_chars(char const* const begin, char const* const end, T& value, int base = 10) noexcept
    {
        return details::int_from_chars_impl(begin, end, value, base);
    }

    template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>>>
    from_chars_result
    from_chars(char const* const begin, char const* const end, T& value, chars_format fmt = chars_format::general) noexcept
    {
        switch (fmt) {
            case chars_format::fixed:
                return details::fp_from_chars_impl(begin, end, value, std::ios::fixed);
            case chars_format::scientific:
                return details::fp_from_chars_impl(begin, end, value, std::ios::scientific);
            case chars_format::hex:
                return details::fp_from_chars_impl(begin, end, value, std::ios::hex);
            case chars_format::general:
                return details::fp_from_chars_impl(begin, end, value, std::ios::dec);
        }
        return {begin, std::errc::invalid_argument};
    }

#endif

} // namespace Raychel

#endif //!RAYCHELCORE_CHARCONV_H