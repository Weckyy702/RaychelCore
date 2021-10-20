#include "AssertingVector.h"
#include <catch2/catch.hpp>

//TODO: extend this test suite once Catch2 allows checking for std::terminate

TEST_CASE("Asserting Vector", "[RaychelCore][Utilities]")
{
    Raychel::AssertingVector<int> v{1, 2, 3, 4, 5};

    REQUIRE(v.size() == 5);

    REQUIRE(v.at(0) == v[0]);
    REQUIRE(v.at(1) == v[1]);
    REQUIRE(v.at(2) == v[2]);
    REQUIRE(v.at(3) == v[3]);
    REQUIRE(v.at(4) == v[4]);
}

TEST_CASE("const Asserting Vector", "[RaychelCore][Utilities]")
{
    const Raychel::AssertingVector<int> v{1, 2, 3, 4, 5};

    REQUIRE(v.size() == 5);

    REQUIRE(v.at(0) == v[0]);
    REQUIRE(v.at(1) == v[1]);
    REQUIRE(v.at(2) == v[2]);
    REQUIRE(v.at(3) == v[3]);
    REQUIRE(v.at(4) == v[4]);
}