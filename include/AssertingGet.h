/*DONT FORGET THE COPYRIGHT ;)*/
#ifndef RAYCHEL_ASSERTING_GET_H
#define RAYCHEL_ASSERTING_GET_H

#include <variant>
#include "Raychel_assert.h"

namespace Raychel {

    //TODO: clang errors on try-catch in constexpr get

    template <typename T, typename... Ts>
    constexpr T& get(std::variant<Ts...>& v) noexcept
    {
        try {
            return std::get<T>(v);
        } catch (std::bad_variant_access&) {
            RAYCHEL_ASSERT_NOT_REACHED;
        }
    }

    template <typename T, typename... Ts>
    constexpr T&& get(std::variant<Ts...>&& v) noexcept
    {
        try {
            return std::get<T>(std::forward<std::variant<Ts...>&&>(v));
        } catch (std::bad_variant_access&) {
            RAYCHEL_ASSERT_NOT_REACHED;
        }
    }

    template <typename T, typename... Ts>
    constexpr const T& get(const std::variant<Ts...>& v) noexcept
    {
        try {
            return std::get<T>(v);
        } catch (std::bad_variant_access&) {
            RAYCHEL_ASSERT_NOT_REACHED;
        }
    }

    template <typename T, typename... Ts>
    constexpr const T&& get(const std::variant<Ts...>&& v) noexcept
    {
        try {
            return std::get<T>(std::forward<std::variant<Ts...>&&>(v));
        } catch (std::bad_variant_access&) {
            RAYCHEL_ASSERT_NOT_REACHED;
        }
    }

} // namespace Raychel

#endif //!RAYCHEL_ASSERTING_GET_H