/**
* \file AssertingVector.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for AssertingVector class
* \date 2021-10-20
*
* \note The methods in this file use reinterpret_cast to convert from Raychel::AssertingVector to its base class std::vector.
*       This is explicitly not UB as stated in the "Notes" section on https://en.cppreference.com/w/cpp/language/reinterpret_cast
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
#ifndef RAYCHELCORE_ASSERTING_VECTOR_H
#define RAYCHELCORE_ASSERTING_VECTOR_H

#include <vector>

#include "Raychel_assert.h"

namespace Raychel {

    template <typename T, typename Allocator = std::allocator<T>>
    struct AssertingVector : public std::vector<T, Allocator>
    {
        using std::vector<T>::vector;

        constexpr T& at(std::size_t pos) noexcept
        {
            RAYCHEL_ASSERT(pos < this->size());
            //NOLINTNEXTLINE: this is a *very* ugly way to forward the call
            return reinterpret_cast<std::vector<T>*>(this)->at(pos);
        }

        constexpr const T& at(std::size_t pos) const noexcept
        {
            RAYCHEL_ASSERT(pos < this->size());
            //NOLINTNEXTLINE: this is a *very* ugly way to forward the call
            return reinterpret_cast<const std::vector<T>*>(this)->at(pos);
        }

        constexpr T& operator[](std::size_t pos) noexcept
        {
            return at(pos);
        }

        constexpr const T& operator[](std::size_t pos) const noexcept
        {
            return at(pos);
        }
    };

} // namespace Raychel

#endif //!RAYCHELCORE_ASSERTING_VECTOR_H