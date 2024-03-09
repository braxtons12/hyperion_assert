// #include <hyperion/platform/def.h>
//
// HYPERION_IGNORE_UNUSED_MACROS_WARNING_START;
//
// #define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
//
// HYPERION_IGNORE_UNUSED_MACROS_WARNING_STOP;
//
// #if HYPERION_PLATFORM_COMPILER_IS_CLANG && __clang_major__ >= 16
// HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_START;
// #endif // HYPERION_PLATFORM_COMPILER_IS_CLANG && __clang_major__ >= 16
//
// #include <hyperion/platform/testing.h>
//
// #if HYPERION_PLATFORM_COMPILER_IS_CLANG && __clang_major__ >= 16
// HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_STOP;
// #endif // HYPERION_PLATFORM_COMPILER_IS_CLANG && __clang_major__ >= 16

// #include <hyperion/source_location.h>

#include <hyperion/assert/panic.h>
#include <hyperion/platform/types.h>
#include <hyperion/source_location.h>

HYPERION_IGNORE_RESERVED_IDENTIFIERS_WARNING_START;
HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_START;
#include <boost/ut.hpp>
HYPERION_IGNORE_UNSAFE_BUFFER_WARNING_STOP;
HYPERION_IGNORE_RESERVED_IDENTIFIERS_WARNING_STOP;

using namespace hyperion; // NOLINT(google-build-using-namespace)

[[nodiscard]] auto
main([[maybe_unused]] i32 argc, [[maybe_unused]] const char* const* argv) -> i32 {
}
