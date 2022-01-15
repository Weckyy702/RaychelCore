/**
* \file Finally.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for Finally class
* \date 2022-01-14
* 
* MIT License
* Copyright (c) [2022] [Weckyy702 (weckyy702@gmail.com | https://github.com/Weckyy702)]
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
#ifndef RAYCHELCORE_FINALLY_H
#define RAYCHELCORE_FINALLY_H

#include "compat.h"

#if __has_include(<concepts>)
    #include <concepts>
    #define RAYCHEL_FINALLY_HAS_CONCEPTS 1
#else
    #include <type_traits>
#endif

#include <utility>

namespace Raychel {

#ifdef RAYCHEL_FINALLY_HAS_CONCEPTS
    template <std::invocable F>
#else
    template <typename F, std::enable_if_t<std::is_invocable_v<F>, bool> = true>
#endif
    class Finally
    {
    public:

        using vaue_type = std::decay_t<F>;

        explicit Finally(F f) : f_{f}
        {}

        Finally(const Finally&) = default;
        Finally(Finally&&) noexcept(std::is_nothrow_move_constructible_v<F>) = default;
        Finally& operator=(const Finally&) = default;
        Finally& operator=(Finally&&) noexcept(std::is_nothrow_move_assignable_v<F>) = default;

        ~Finally() noexcept(std::is_nothrow_invocable_v<F>&& std::is_nothrow_destructible_v<F>)
        {
            f_();
        }

    private:
        vaue_type f_;
    };

} //namespace RaychelCore

#endif //!RAYCHELCORE_FINALLY_H