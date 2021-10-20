/**
* \file AssertingOptional.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for AssertingOptional class
* \date 2021-09-29
* \note The methods in this file use reinterpret_cast to convert from Raychel::AssertingOptional to its base class std::optional.
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
#ifndef RAYCHELCORE_ASSERTING_OPTIONAL_H
#define RAYCHELCORE_ASSERTING_OPTIONAL_H

#include <optional>
#include "Raychel_assert.h"

namespace Raychel {

    template <typename T>
    struct AssertingOptional : std::optional<T>
    {
        using std::optional<T>::optional;

        constexpr const T* operator->() const noexcept
        {
            RAYCHEL_ASSERT(this->has_value());
            return reinterpret_cast<std::optional<T>*>(this)->operator->(); //NOLINT: this is a very ugly way to forward the call
        }

        constexpr T* operator->() noexcept
        {
            RAYCHEL_ASSERT(this->has_value());
            return reinterpret_cast<std::optional<T>*>(this)->operator->(); //NOLINT: this is a very ugly way to forward the call
        }

        constexpr const T& operator*() const& noexcept
        {
            RAYCHEL_ASSERT(this->has_value());
            //NOLINTNEXTLINE: this is a very ugly way to forward the call
            return reinterpret_cast<const std::optional<T>*>(this)->operator*();
        }

        constexpr T& operator*() & noexcept
        {
            RAYCHEL_ASSERT(this->has_value());
            return reinterpret_cast<std::optional<T>*>(this)->operator*(); //NOLINT: this is a very ugly way to forward the call
        }

        constexpr const T&& operator*() const&& noexcept //is a const T&& even a thing?
        {
            RAYCHEL_ASSERT(this->has_value());
            //NOLINTNEXTLINE: this is a very ugly way to forward the call
            return reinterpret_cast<const std::optional<T>*>(this)->operator*();
        }

        constexpr T&& operator*() && noexcept
        {
            RAYCHEL_ASSERT(this->has_value());
            //NOLINTNEXTLINE: this is a *very* ugly way to forward the call
            return std::move(*reinterpret_cast<std::optional<T>*>(this)).operator*();
        }

        constexpr const T& value() const& noexcept
        {
            RAYCHEL_ASSERT(this->has_value());
            return reinterpret_cast<const std::optional<T>*>(this)->value(); //NOLINT: this is a very ugly way to forward the call
        }

        constexpr T& value() & noexcept
        {
            RAYCHEL_ASSERT(this->has_value());
            return reinterpret_cast<std::optional<T>*>(this)->value(); //NOLINT: this is a very ugly way to forward the call
        }
    };

} // namespace Raychel

#endif //!RAYCHELCORE_ASSERTING_OPTIONAL_H