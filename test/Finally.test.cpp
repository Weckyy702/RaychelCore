#include "RaychelCore/Finally.h"
#include <catch2/catch.hpp>

TEST_CASE("Finally: Lambda")
{
    using Raychel::Finally;

    int test{};

    REQUIRE(test == 0);

    {
        Finally _{[&] { test = 1234; }};
    }

    REQUIRE(test == 1234);
}

int g_test{};

void mutate_state() noexcept
{
    g_test = 4321;
}

void reset_state() noexcept
{
    g_test = 0;
}

TEST_CASE("Finally: Free Function")
{
    using Raychel::Finally;

    Finally _1{reset_state};

    REQUIRE(g_test == 0);

    {
        Finally _{mutate_state};
    }

    REQUIRE(g_test == 4321);
}

int meaning_of_life() noexcept
{
    using Raychel::Finally;

    Finally _{mutate_state};

    return 42;
}

TEST_CASE("Finally: normal function return")
{
    using Raychel::Finally;

    Finally _1{reset_state};

    REQUIRE(g_test == 0);

    meaning_of_life();

    REQUIRE(g_test == 4321);
}

void solve_halting_problem()
{
    using Raychel::Finally;

    Finally _{mutate_state};

    throw std::runtime_error{"No solution :(("};
}

TEST_CASE("Finally: return via exception")
{
    using Raychel::Finally;

    Finally _1{reset_state};

    REQUIRE(g_test == 0);

    try{
        solve_halting_problem();
    }catch(...) {
        //not solvable :(
    }

    REQUIRE(g_test == 4321);
}