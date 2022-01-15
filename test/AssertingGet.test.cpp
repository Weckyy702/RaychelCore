#include "RaychelCore/AssertingGet.h"
#include <catch2/catch.hpp>

//TODO: extend this test suite once Catch2 allows checking for std::terminate

TEST_CASE("AssertingGet", "[RaychelCore][Utilities]")
{
    std::variant<int, double> v{1.0};
    const auto d = Raychel::get<double>(v);
    REQUIRE(d == 1.0);
}

TEST_CASE("const AssertingGet", "[RaychelCore][Utilities]")
{
    const std::variant<int, double> v{1.0};
    const auto d = Raychel::get<double>(v);
    REQUIRE(d == 1.0);
}

TEST_CASE("rvalue AssertingGet", "[RaychelCore][Utilities]")
{
    const auto d = Raychel::get<double>(std::variant<int, double>{1.0});
    REQUIRE(d == 1.0);
}

TEST_CASE("constexpr AssertingGet", "[RaychelCore][Utilities]")
{
    constexpr std::variant<int, double> v{1.0};
    constexpr auto d = Raychel::get<double>(v);
    REQUIRE(d == 1.0);
}