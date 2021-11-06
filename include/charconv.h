/*DONT FORGET THE COPYRIGHT ;)*/
#ifndef RAYCHELCORE_CHARCONV_H
#define RAYCHELCORE_CHARCONV_H

#include <system_error>

#include "./compat.h"

#if RAYCHEL_HAS_CHARCONV
    #include <charconv>
#else
    #include <sstream>
    #include <string>
#endif

namespace RaychelCore {

#if RAYCHEL_HAS_CHARCONV
    using std::from_chars_result;
#else
    struct from_chars_result
    {
        const char* ptr;
        std::errc ec;
    };
#endif

#if RAYCHEL_HAS_CHARCONV
    using std::from_chars;
#else
    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    from_chars_result from_chars(char const* const begin, char const* const end, T& value) noexcept
    {
        std::istringstream stream{std::string{begin, end}};

        if (T val; (stream >> val)) {
            value = val;
            return {end, {}};
        }

        return {begin, std::errc::invalid_argument};
    }
#endif

} //namespace RaychelCore

#endif //!RAYCHELCORE_CHARCONV_H