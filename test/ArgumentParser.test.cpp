#include "ArgumentParser.h"
#include <catch2/catch.hpp>

TEST_CASE("ArgumentParser", "[RaychelCore][ArgumentParser]")
{
    Logger::disableColor();
    //NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays): C-Style string are pain
    const char* _argv[] = {
        "/path/to/binary",
        "--iterations",
        "5",
        "-n",
        "10",
        "one_more_option_just_for_the_fun_of_it"
    };
    const int _argc = sizeof(_argv) / sizeof(_argv[0]);

    struct{
        int i{0};
        float f{0};
        std::string str;
    } state;

    Raychel::ArgumentParser ap;

    REQUIRE(ap.add_int_arg("iterations", "n", "number of iterations", state.i));
    REQUIRE(!ap.add_string_arg("iterations", "n", "number of iterations, again", state.str));

    //NOLINTNEXTLINE(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay): C-Style string are pain
    REQUIRE(ap.parse(_argc, _argv));

    REQUIRE(state.i == 10);
    REQUIRE(state.f == 0.0);
    REQUIRE(state.str.empty());
}