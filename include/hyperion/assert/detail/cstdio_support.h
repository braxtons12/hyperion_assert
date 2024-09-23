/// @file cstdio_support.h
/// @author Braxton Salyer <braxtonsalyer@gmail.com>
/// @brief Helper functions for determining properties about C file pointers
/// @version 0.1
/// @date 2024-09-22
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

#ifndef HYPERION_ASSERT_DETAIL_CSTDIO_SUPPORT_H
#define HYPERION_ASSERT_DETAIL_CSTDIO_SUPPORT_H

#include <cstdio>

namespace hyperion::assert::detail::cstdio_support {
    /// @brief Returns whether the given file descriptor identifies a TTY
    /// @param file_descriptor The file descriptor to check
    /// @return Whether `file_descriptor` identifies a TTY
    /// @ingroup cstdio_support
    /// @headerfile hyperion/assert/detail/cstdio_support.h
    [[nodiscard]] auto isatty(int file_descriptor) noexcept -> bool;
    /// @brief Returns the file descriptor number of the given `FILE`
    /// @param file The `FILE` to get the descriptor for
    /// @return The file descriptor number of `file`
    /// @ingroup cstdio_support
    /// @headerfile hyperion/assert/detail/cstdio_support.h
    [[nodiscard]] auto fileno(std::FILE* file) noexcept -> int;
} // namespace hyperion::assert::detail::cstdio_support

#endif // HYPERION_ASSERT_DETAIL_CSTDIO_SUPPORT_H
