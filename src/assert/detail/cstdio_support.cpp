/// @file cstdio_support.cpp
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Helper functions for determining properties about C file pointers
/// @version 0.1
/// @date 2024-03-15
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

#include <hyperion/assert/detail/cstdio_support.h>
#include <hyperion/platform.h>

#include <cstdio>

#if HYPERION_PLATFORM_IS_WINDOWS
    #include <io.h>
#else
    #include <unistd.h>
#endif

namespace hyperion::assert::detail::cstdio_support {
    [[nodiscard]] auto isatty(int desc) noexcept -> bool {
#if HYPERION_PLATFORM_IS_WINDOWS
        return _isatty(desc) != 0;
#else
        return ::isatty(desc) != 0;
#endif // HYPERION_PLATFORM_IS_WINDOWS
    }

    [[nodiscard]] auto fileno(std::FILE* file) noexcept -> int {
#if HYPERION_PLATFORM_IS_WINDOWS
        return _fileno(file);
#else
        return ::fileno(file);
#endif // HYPERION_PLATFORM_IS_WINDOWS
    }
} // namespace hyperion::assert::detail::cstdio_support
