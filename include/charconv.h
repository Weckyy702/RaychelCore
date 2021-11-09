/*DONT FORGET THE COPYRIGHT ;)*/
#ifndef RAYCHELCORE_CHARCONV_H
#define RAYCHELCORE_CHARCONV_H

#include <system_error>
#include "./compat.h"

#if RAYCHEL_HAS_CHARCONV
    #include <charconv>
#else
    #pragma message("IMPORTANT: You are using RaychelCore's replacement for std::from_chars. The integer version only supports the bases 2, 8, 10 and 16!")
    #include <sstream>
    #include <iomanip>
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

        #if RAYCHEL_VERSION_CHECK(202002L)
        bool operator==(const from_chars_result& b) const noexcept = default;
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
        from_chars_result fp_from_chars_impl(char const* const begin, char const* const end, T& value, std::ios::fmtflags format) noexcept
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
            if (base != 2 && base != 8 && base != 10 && base != 16) {
                return {begin, std::errc::invalid_argument};
            }


            std::stringstream stream{std::string{begin, end}};
            stream.imbue(std::locale::classic());
            stream << std::setbase(base);

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
    from_chars_result from_chars(char const* const begin, char const* const end, T& value, chars_format fmt = chars_format::general) noexcept
    {
        switch(fmt) {
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

} //namespace RaychelCore

#endif //!RAYCHELCORE_CHARCONV_H