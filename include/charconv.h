/*DONT FORGET THE COPYRIGHT ;)*/
#ifndef RAYCHELCORE_CHARCONV_H
#define RAYCHELCORE_CHARCONV_H

#include <system_error>
#include "./compat.h"

#if RAYCHEL_HAS_CHARCONV
    #include <charconv>
#else
    #pragma message "IMPORTANT: You are using RaychelCore's replacement for std::from_chars. The integer version only supports the base 2, 8, 10 and 16!"
    #include <sstream>
    #include <string>
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
        template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
        from_chars_result from_chars_impl(char const* const begin, char const* const end, T& value, std::ios::fmtflags format) noexcept
        {
            std::istringstream stream{std::string{begin, end}};
            stream.setf(format);

            if (T val; (stream >> val)) {
                value = val;
                return {end, {}};
            }

            return {begin, std::errc::invalid_argument};
        }
    } // namespace details

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    from_chars_result from_chars(char const* const begin, char const* const end, T& value) noexcept
    {
        return details::from_chars_impl(begin, end, value, std::ios::dec);
    }

    template <typename T, typename std::enable_if_t<std::is_floating_point_v<T>>>
    from_chars_result from_chars(char const* const begin, char const* const end, T& value, chars_format fmt) noexcept
    {
        switch(fmt) {
            case chars_format::fixed:
                return details::from_chars_impl(begin, end, value, std::ios::fixed);
            case chars_format::scientific:
                return details::from_chars_impl(begin, end, value, std::ios::scientific);
            case chars_format::hex:
                return details::from_chars_impl(begin, end, value, std::ios::hex);
            case chars_format::general:
                return details::from_chars_impl(begin, end, value, std::ios::dec);
        }
        return {begin, std::errc::invalid_argument};
    }

#endif

} //namespace RaychelCore

#endif //!RAYCHELCORE_CHARCONV_H