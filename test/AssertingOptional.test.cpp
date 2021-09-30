#include <catch2/catch.hpp>
#include "AssertingOptional.h"

//TODO: extend this test suite once Catch2 allows checking for std::terminate

TEST_CASE("Asserting Optional", "[RaychelCore][Utilities]")
{
    Raychel::AssertingOptional<int> opt;
    opt.emplace(5);

    REQUIRE(opt.has_value());
    REQUIRE(opt.value() == 5);
    REQUIRE(*opt == 5);
}

TEST_CASE("const Asserting Optional", "[RaychelCore][Utilities]")
{
    const Raychel::AssertingOptional<int> opt{5};

    REQUIRE(opt.has_value());
    REQUIRE(opt.value() == 5);
    REQUIRE(*opt == 5);
}

TEST_CASE("rvalue Asserting Optional", "[RaychelCore][Utilities]")
{
    using Raychel::AssertingOptional;

    REQUIRE(AssertingOptional<int>{5}.has_value());
    REQUIRE(AssertingOptional<int>{5}.value() == 5);
    REQUIRE(*AssertingOptional<int>{5} == 5);
}