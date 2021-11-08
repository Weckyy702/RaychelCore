#define RAYCHELCORE_USE_CHARCONV_REPLACEMENT
#include "charconv.h"
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
}

TEST_CASE("Raychel scientific charconv", "[RaychelCore][Compatibility]")
{
    constexpr std::string_view decimal_float = "1.2345";
    constexpr std::string_view decimal_int = "12345";

    SECTION("Double decimal from_chars")
    {
        double d{0};
        const auto [_, ec] = Raychel::from_chars(decimal_float.data(), decimal_float.data() + decimal_float.size(), d, Raychel::chars_format::scientific);

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
}
//NOLINTEND