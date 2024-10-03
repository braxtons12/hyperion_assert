/// @file main.cpp
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Basic usage example of hyperion::assert.
/// @version 0.1
/// @date 2024-10-02
///
/// MIT License
/// @copyright Copyright (c) 2024 Braxton Salyer <braxtonsalyer@gmail.com>
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.

#include <hyperion/assert.h>
#include <hyperion/platform/types.h>

#if HYPERION_PLATFORM_COMPILER_IS_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-overflow"
#endif // HYPERION_PLATFORM_COMPILER_IS_GCC

#include <fmt/format.h>

#if HYPERION_PLATFORM_COMPILER_IS_GCC
    #pragma GCC diagnostic pop
#endif // HYPERION_PLATFORM_COMPILER_IS_GCC

using namespace hyperion; // NOLINT(google-build-using-namespace)
                          
[[nodiscard]] auto
main([[maybe_unused]] i32 argc, [[maybe_unused]] const char* const* argv) -> i32 {
    const auto lambda = []() {
        return 5;
    };
    HYPERION_ASSERT_DEBUG(2 + lambda() == 7, "lambda() wasn't {}", 5);
    return 0_i32;
}
