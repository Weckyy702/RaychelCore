/**
* \file ArgumentParser.test.cpp
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Testing file for ArgumentParser class
* \date 2021-09-20
* 
* MIT License
* Copyright (c) [2021] [Weckyy702 (weckyy702@gmail.com | https://github.com/Weckyy702)]
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
* 
*/
#include "ArgumentParser.h"
#include "catch2/catch.hpp"

//NOLINTNEXTLINE(readability-function-cognitive-complexity)
TEST_CASE("ArgumentParser", "[RaychelCore][ArgumentParser]")
{
    //NOLINTNEXTLINE(hicpp-avoid-c-arrays, modernize-avoid-c-arrays, cppcoreguidelines-avoid-c-arrays): C-Style strings are pain
    const char* _argv[] = {
        "/path/to/binary", "--iterations", "5", "-n", "10", "-f", "-123.45", "one_more_option_just_for_the_fun_of_it"};
    const int _argc = sizeof(_argv) / sizeof(_argv[0]);

    struct
    {
        int i{0};
        float f{0};
        std::string str;
    } state;

    Raychel::ArgumentParser ap;

    REQUIRE(ap.add_float_arg("arg", "f", "float arg", state.f));
    REQUIRE(ap.add_int_arg("iterations", "n", "number of iterations", state.i));
    REQUIRE(!ap.add_string_arg("iterations", "n", "number of iterations, again", state.str));

    //NOLINTNEXTLINE(hicpp-no-array-decay, cppcoreguidelines-pro-bounds-array-to-pointer-decay): C-Style strings are pain
    REQUIRE(ap.parse(_argc, _argv));

    REQUIRE(state.i == 10);
    REQUIRE(state.f == -123.45F);
    REQUIRE(state.str.empty());
}
