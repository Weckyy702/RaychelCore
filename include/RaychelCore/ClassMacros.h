/**
* \file ClassMacros.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief definition for class macros
* \date 2021-10-01
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
#ifndef RAYCHELCORE_CLASS_MACROS_H
#define RAYCHELCORE_CLASS_MACROS_H

#define RAYCHEL_DELETE_COPY_CTOR(type_name) type_name(const type_name&) = delete;

#define RAYCHEL_DELETE_COPY_OP(type_name) type_name& operator=(const type_name&) = delete;

//NOLINTNEXTLINE: the type name must not be enclosed in parantheses
#define RAYCHEL_DELETE_MOVE_CTOR(type_name) type_name(type_name&&) = delete;

//NOLINTNEXTLINE: the type name must not be enclosed in parantheses
#define RAYCHEL_DELETE_MOVE_OP(type_name) type_name& operator=(type_name&&) = delete;

#define RAYCHEL_MAKE_NONCOPY(type_name)                                                                                          \
    RAYCHEL_DELETE_COPY_CTOR(type_name)                                                                                          \
    RAYCHEL_DELETE_COPY_OP(type_name)

#define RAYCHEL_MAKE_NONMOVE(type_name)                                                                                          \
    RAYCHEL_DELETE_MOVE_CTOR(type_name)                                                                                          \
    RAYCHEL_DELETE_MOVE_OP(type_name)

#define RAYCHEL_MAKE_NONCOPY_NONMOVE(type_name)                                                                                  \
    RAYCHEL_MAKE_NONCOPY(type_name)                                                                                              \
    RAYCHEL_MAKE_NONMOVE(type_name)

#define RAYCHEL_DEFAULT_COPY_CTOR(type_name) type_name(const type_name&) = default;

#define RAYCHEL_DEFAULT_COPY_OP(type_name) type_name& operator=(const type_name&) = default;

//NOLINTNEXTLINE: the type name must not be enclosed in parantheses
#define RAYCHEL_DEFAULT_MOVE_CTOR(type_name) type_name(type_name&&) = default;

//NOLINTNEXTLINE: the type name must not be enclosed in parantheses
#define RAYCHEL_DEFAULT_MOVE_OP(type_name) type_name& operator=(type_name&&) = default;

#define RAYCHEL_MAKE_DEFAULT_COPY(type_name)                                                                                     \
    RAYCHEL_DEFAULT_COPY_CTOR(type_name)                                                                                         \
    RAYCHEL_DEFAULT_COPY_OP(type_name)

#define RAYCHEL_MAKE_DEFAULT_MOVE(type_name)                                                                                     \
    RAYCHEL_DEFAULT_MOVE_CTOR(type_name)                                                                                         \
    RAYCHEL_DEFAULT_MOVE_OP(type_name)

#define RAYCHEL_CONCAT_IMPL(x, y) x##y
#define RAYCHEL_CONCAT(x, y) RAYCHEL_CONCAT_IMPL(x, y)
#define RAYCHEL_ANON_VAR [[maybe_unused]] const volatile auto RAYCHEL_CONCAT(n, __LINE__) =

#endif //!RAYCHELCORE_CLASS_MACROS_H