/**
* \file CommandLineValueReference.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief 
* \date 2021-09-25
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
#ifndef RAYCHEL_CORE_COMMAND_LINE_VALUE_REFERENCE_H
#define RAYCHEL_CORE_COMMAND_LINE_VALUE_REFERENCE_H

#include <functional>
#include <string>

#include "Raychel_assert.h"

namespace Raychel {
    class CommandLineValueReference
    {

    public:
        enum class ValueType { int_, float_, string_ };

        explicit CommandLineValueReference(int& value) //NOLINT(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
            : type_{ValueType::int_}, as_int_{value}
        {}

        explicit CommandLineValueReference(float& value) //NOLINT(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
            : type_{ValueType::float_}, as_float_{value}
        {}

        explicit CommandLineValueReference(std::string& value) //NOLINT(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
            : type_{ValueType::string_}, as_string_{value}
        {}

        //NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
        CommandLineValueReference(const CommandLineValueReference& rhs) : type_{rhs.type_}
        {
            _assign_from(rhs);
        }

        //NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
        CommandLineValueReference(CommandLineValueReference&& rhs) noexcept : type_{rhs.type_}
        {
            _assign_from(rhs);
        }

        CommandLineValueReference& operator=(const CommandLineValueReference& rhs)
        {
            type_ = rhs.type_;
            _assign_from(rhs);

            return *this;
        }

        CommandLineValueReference& operator=(CommandLineValueReference&& rhs) noexcept
        {
            type_ = rhs.type_;
            _assign_from(rhs);

            return *this;
        }

        [[nodiscard]] ValueType type() const noexcept
        {
            return type_;
        }

        [[nodiscard]] int& as_int_ref() const noexcept
        {
            RAYCHEL_ASSERT(type() == ValueType::float_);
            return as_int_; //NOLINT(cppcoreguidelines-pro-type-union-access)
        }

        [[nodiscard]] float& as_float_ref() const noexcept
        {
            RAYCHEL_ASSERT(type() == ValueType::float_);
            return as_float_; //NOLINT(cppcoreguidelines-pro-type-union-access)
        }

        [[nodiscard]] std::string& as_string_ref() const noexcept
        {
            RAYCHEL_ASSERT(type() == ValueType::float_);
            return as_string_; //NOLINT(cppcoreguidelines-pro-type-union-access)
        }

        //since std::reference_wrapper contains a non-owning pointer, this should be ok
        ~CommandLineValueReference() = default;

    private:
        void _assign_from(const CommandLineValueReference& rhs) noexcept
        {
            //This might cause UB. let's find out in testing ;)
            as_int_ = rhs.as_int_; //NOLINT(cppcoreguidelines-pro-type-union-access)
        }

        ValueType type_;
        union
        {
            std::reference_wrapper<int> as_int_;
            std::reference_wrapper<float> as_float_;
            std::reference_wrapper<std::string> as_string_;
        };
    };

} // namespace Raychel
#endif //!RAYCHEL_CORE_COMMAND_LINE_VALUE_REFERENCE_H