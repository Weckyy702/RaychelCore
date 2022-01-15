#define RAYCHELCORE_USE_CHARCONV_REPLACEMENT 1
#include "RaychelCore/charconv.h"
#include <catch2/catch.hpp>
#include <string_view>

//NOLINTBEGIN

TEST_CASE("Raychel decimal charconv", "[RaychelCore][Compatibility]")
{
    constexpr std::string_view decimal_float = "1.2345";
    constexpr std::string_view decimal_int = "12345";

    SECTION("Double decimal from_chars")
    {
        double d{0};
        const auto [_, ec] = Raychel::from_chars(decimal_float.data(), decimal_float.data() + decimal_float.size(), d);

        REQUIRE(ec == std::errc{});
        REQUIRE(Catch::compareEqual(d, 1.2345));
    }

    SECTION("Single decimal from_chars")
    {
        float d{0};
        const auto [_, ec] = Raychel::from_chars(decimal_float.data(), decimal_float.data() + decimal_float.size(), d);

        REQUIRE(ec == std::errc{});
        REQUIRE(Catch::compareEqual(d, 1.2345F));
    }

    SECTION("Integer decimal from_chars")
    {
        int i{0};
        const auto [_, ec] = Raychel::from_chars(decimal_int.data(), decimal_int.data() + decimal_int.size(), i);

        REQUIRE(ec == std::errc{});
        REQUIRE(i == 12345);
    }

    SECTION("Unsigned decimal from_chars")
    {
        unsigned i{0};
        const auto [_, ec] = Raychel::from_chars(decimal_int.data(), decimal_int.data() + decimal_int.size(), i);

        REQUIRE(ec == std::errc{});
        REQUIRE(i == 12345U);
    }

    SECTION("Long decimal from_chars")
    {
        std::uint64_t i{0};
        const auto [_, ec] = Raychel::from_chars(decimal_int.data(), decimal_int.data() + decimal_int.size(), i);

        REQUIRE(ec == std::errc{});
        REQUIRE(i == 12345L);
    }

    //TODO: add tests for to_chars
}

TEST_CASE("Raychel octal integer charconv", "[RaychelCore][Compatibility]")
{
    constexpr std::string_view octal_int = "176304120";
    SECTION("Integer octal from_chars")
    {
        int i{0};
        const auto [_, ec] = Raychel::from_chars(octal_int.data(), octal_int.data() + octal_int.size(), i, 8);

        REQUIRE(ec == std::errc{});
        REQUIRE(i == 0176304120);
    }

    SECTION("Unsigned octal from_chars")
    {
        unsigned i{0};
        const auto [_, ec] = Raychel::from_chars(octal_int.data(), octal_int.data() + octal_int.size(), i, 8);

        REQUIRE(ec == std::errc{});
        REQUIRE(i == 0176304120);
    }

    SECTION("Long octal from_chars")
    {
        std::uint64_t i{0};
        const auto [_, ec] = Raychel::from_chars(octal_int.data(), octal_int.data() + octal_int.size(), i, 8);

        REQUIRE(ec == std::errc{});
        REQUIRE(i == 0176304120);
    }
}

TEST_CASE("Raychel hex integer charconv", "[RaychelCore][Compatibility]")
{
    constexpr std::string_view hexadecimal_int = "BADB002";
    SECTION("Integer hex from_chars")
    {
        int i{0};
        const auto [_, ec] = Raychel::from_chars(hexadecimal_int.data(), hexadecimal_int.data() + hexadecimal_int.size(), i, 16);

        REQUIRE(ec == std::errc{});
        REQUIRE(i == 0xBADB002);
    }

    SECTION("Unsigned hex from_chars")
    {
        unsigned i{0};
        const auto [_, ec] = Raychel::from_chars(hexadecimal_int.data(), hexadecimal_int.data() + hexadecimal_int.size(), i, 16);

        REQUIRE(ec == std::errc{});
        REQUIRE(i == 0xBADB002);
    }

    SECTION("Long hex from_chars")
    {
        std::uint64_t i{0};
        const auto [_, ec] = Raychel::from_chars(hexadecimal_int.data(), hexadecimal_int.data() + hexadecimal_int.size(), i, 16);

        REQUIRE(ec == std::errc{});
        REQUIRE(i == 0xBADB002);
    }
}
//NOLINTEND