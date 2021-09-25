/**
* \file Timer.h
* \author Weckyy702 (weckyy702@gmail.com)
* \brief Header file for ScopedTimer class
* \date 2021-09-03
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
#ifndef RAYCHEL_CORE_TIMER_H
#define RAYCHEL_CORE_TIMER_H

#include <string_view>
#include "RaychelLogger/Logger.h"

namespace Raychel {

    class ScopedTimer
    {
    public:
        explicit ScopedTimer(std::string_view label)
        {
            label_ = Logger::startTimer(label);
        }

        ScopedTimer(const ScopedTimer&) = delete;
        ScopedTimer(ScopedTimer&&) = delete;
        ScopedTimer& operator=(const ScopedTimer&) = delete;
        ScopedTimer& operator=(ScopedTimer&&) = delete;

        ~ScopedTimer() noexcept
        {
            Logger::logDuration(label_);
        }

    private:
        std::string label_;
    };

} // namespace Raychel

#endif //!RAYCHEL_CORE_TIMER_H