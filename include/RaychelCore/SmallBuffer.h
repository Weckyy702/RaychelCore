/**
* \file SmallBuffer.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for SmallBuffer class
* \date 2022-12-26
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
#ifndef RAYCHELCORE_SMALL_BUFFER_H
#define RAYCHELCORE_SMALL_BUFFER_H

#include "ClassMacros.h"
#include "Raychel_assert.h"

#include <cstddef>
#include <cstring>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>

namespace Raychel {
    namespace details {
        template <typename T>
        struct SmallBufferImpl
        {
            static void do_destroy(void* _ptr, bool do_delete) noexcept
            {
                auto* ptr = std::launder(reinterpret_cast<T*>(_ptr));
                ptr->~T();

                if (do_delete)
                    operator delete(ptr);
            }

            static void do_copy(const void* _src, void* dst) noexcept
            {
                const auto* src = std::launder(reinterpret_cast<const T*>(_src));

                new (dst) T{*src};
            }
        };
    } // namespace details

    template <std::size_t BufferSize>
    class SmallBuffer
    {
        using DestroyFunction = void (*)(void*, bool) noexcept;
        using CopyFunction = void (*)(const void*, void*) noexcept;

    public:
        SmallBuffer() = default;

        template <typename T>
        requires(!std::is_same_v<std::remove_cvref_t<T>, SmallBuffer>) explicit SmallBuffer(T&& value)
        {
            emplace(std::forward<T>(value));
        }

        SmallBuffer(SmallBuffer&& other) noexcept
            : do_destroy_{other.do_destroy_}, do_copy_{other.do_copy_}, object_size_(other.object_size_)
        {
            _move(std::move(other));
        }

        SmallBuffer(const SmallBuffer& other)
            : do_destroy_{other.do_destroy_}, do_copy_{other.do_copy_}, object_size_{other.object_size_}
        {
            _copy(other);
        }

        SmallBuffer& operator=(SmallBuffer&& other) noexcept
        {
            if (this == std::addressof(other))
                return *this;

            _destroy();

            do_destroy_ = other.do_destroy_;
            do_copy_ = other.do_copy_;
            object_size_ = other.object_size_;

            _move(std::move(other));

            return *this;
        }

        SmallBuffer& operator=(const SmallBuffer& other) noexcept
        {
            if (this == std::addressof(other))
                return *this;

            _destroy();

            do_destroy_ = other.do_destroy_;
            do_copy_ = other.do_copy_;
            object_size_ = other.object_size_;

            _copy(other);

            return *this;
        }

        template <typename T>
        requires std::is_copy_constructible_v<T> T& emplace(T&& value)
        {
            object_size_ = sizeof(T);
            _destroy();

            if constexpr (sizeof(T) <= BufferSize) {
                ptr_ = &buffer_;
            } else {
                ptr_ = operator new(object_size_);
            }

            ::new (ptr_) T{std::forward<T>(value)};
            do_destroy_ = details::SmallBufferImpl<T>::do_destroy;
            do_copy_ = details::SmallBufferImpl<T>::do_copy;

            return unsafe_get_value<T>();
        }

        template <typename T>
        [[nodiscard]] T& unsafe_get_value() noexcept
        {
            RAYCHEL_ASSERT(ptr_ != nullptr);
            return *std::launder(reinterpret_cast<T*>(ptr_));
        }

        template <typename T>
        [[nodiscard]] const T& unsafe_get_value() const noexcept
        {
            RAYCHEL_ASSERT(ptr_ != nullptr);
            return *std::launder(reinterpret_cast<const T*>(ptr_));
        }

        [[nodiscard]] void* unsafe_get_ptr() noexcept
        {
            return ptr_;
        }

        [[nodiscard]] const void* unsafe_get_ptr() const noexcept
        {
            return ptr_;
        }

        [[nodiscard]] bool is_heap() const noexcept
        {
            return ptr_ != &buffer_;
        }

        ~SmallBuffer() noexcept
        {
            _destroy();
        }

    private:
        void _destroy() noexcept
        {
            if (ptr_ == nullptr)
                return;
            do_destroy_(ptr_, is_heap());
        }

        void _copy(const SmallBuffer& other) noexcept
        {
            if (!other.is_heap()) {
                ptr_ = &buffer_;
            } else {
                ptr_ = operator new(object_size_);
            }

            do_copy_(other.ptr_, ptr_);
        }

        void _move(SmallBuffer&& other) noexcept
        {
            if (!other.is_heap()) {
                ptr_ = &buffer_;
                std::memcpy(ptr_, other.ptr_, object_size_);
                std::memset(other.ptr_, 0, object_size_);
            } else {
                ptr_ = other.ptr_;
            }

            other.ptr_ = nullptr;
        }

        void* ptr_{};
        DestroyFunction do_destroy_{};
        CopyFunction do_copy_{};
        std::size_t object_size_{};
        std::aligned_storage_t<BufferSize> buffer_{};
    };
} // namespace Raychel

#endif //!RAYCHELCORE_SMALL_BUFFER_H
