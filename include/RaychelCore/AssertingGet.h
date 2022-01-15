/*DONT FORGET THE COPYRIGHT ;)*/
#ifndef RAYCHEL_ASSERTING_GET_H
#define RAYCHEL_ASSERTING_GET_H

#include <variant>
#include "Raychel_assert.h"

namespace Raychel {

    //TODO: clang errors on try-catch in constexpr get

    template <typename T, typename... Ts>
    constexpr T& get(std::variant<Ts...>& variant) noexcept
    {
        RAYCHEL_ASSERT(std::holds_alternative<T>(variant))
        return std::get<T>(variant);
    }

    template <typename T, typename... Ts>
    constexpr T&& get(std::variant<Ts...>&& variant) noexcept
    {
        RAYCHEL_ASSERT(std::holds_alternative<T>(variant))
        return std::get<T>(std::move(variant));
    }

    template <typename T, typename... Ts>
    constexpr const T& get(const std::variant<Ts...>& variant) noexcept
    {
        RAYCHEL_ASSERT(std::holds_alternative<T>(variant))
        return std::get<T>(variant);
    }

    template <typename T, typename... Ts>
    constexpr const T&& get(const std::variant<Ts...>&& variant) noexcept
    {
        RAYCHEL_ASSERT(std::holds_alternative<T>(variant))
        return std::get<T>(variant);
    }

} // namespace Raychel

#endif //!RAYCHEL_ASSERTING_GET_H