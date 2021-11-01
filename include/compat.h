/**
*\file compat.h
*\author weckyy702 (weckyy702@gmail.com)
*\brief Compatibility defines
*\date 2021-06-15
*
*MIT License
*Copyright (c) [2021] [Weckyy702 (weckyy702@gmail.com | https://github.com/Weckyy702)]
*Permission is hereby granted, free of charge, to any person obtaining a copy
*of this software and associated documentation files (the "Software"), to deal
*in the Software without restriction, including without limitation the rights
*to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*copies of the Software, and to permit persons to whom the Software is
*furnished to do so, subject to the following conditions:
*
*The above copyright notice and this permission notice shall be included in all
*copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*SOFTWARE.
*
*/
#ifndef RAYCHEL_COMPAT_H
#define RAYCHEL_COMPAT_H

#include <version>

#if defined(__clang__) || defined(__GNUC__)
    #define RAYCHEL_FUNC_NAME __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
    #define RAYCHEL_FUNC_NAME __FUNCSIG__
#else
    #define RAYCHEL_FUNC_NAME __func__
#endif

#ifdef __GNUC__
    #define RAYCHEL_ACTIVE_COMPILER ::Raychel::compiler::gcc
#elif defined(__clang__)
    #define RAYCHEL_ACTIVE_COMPILER ::Raychel::compiler::clang
#elif defined(_MSVC_VER)
    #define RAYCHEL_ACTIVE_COMPILER ::Raychel::compiler::msvc
#else
    #pragma message("Unable to detect compiler id!")
    #define RAYCHEL_ACTIVE_COMPILER ::Raychel::compiler::unknown
#endif

#ifdef __linux
    #define RAYCHEL_ACTIVE_OS ::Raychel::OS::Linux
#elif defined(_WIN32)
    #define RAYCHEL_ACTIVE_OS ::Raychel::OS::Win32
#elif defined(__APPLE__)
    #define RAYCHEL_ACTIVE_OS ::Raychel::OS::Darwin
    #pragma message("Please keep in mind that this version of RaychelCore is not tested on MacOS")
#else
    #pragma message("Unable to detect host OS!")
    #define RAYCHEL_ACTIVE_OS ::Raychel::OS::unknown
#endif

#if __cpp_lib_three_way_comparison >= 201907L
    #define RAYCHEL_HAS_SPACESHIP_OP 1
#else
    #define RAYCHEL_HAS_SPACESHIP_OP 0
#endif

#if __cpp_lib_to_chars >= 201611L
    #define RAYCHEL_HAS_TO_CHARS 1
#else
    #define RAYCHEL_HAS_TO_CHARS 0
#endif


namespace Raychel {
    enum class OS {
        unknown,
        Win32,
        Linux,
        Darwin,
    };

    enum class compiler {
        unknown,
        gcc,
        clang,
        msvc,
    };
} //namespace Raychel

#endif //!RAYCHEL_COMPAT_H