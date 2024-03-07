cmake_minimum_required(VERSION 3.20)
project(hyperion_assert LANGUAGES CXX)

include(CTest)

option(HYPERION_ENABLE_TRACY "Enables Profiling with Tracy" OFF)
option(HYPERION_USE_FETCH_CONTENT "Enables FetchContent usage for getting dependencies" ON)

set(HYPERION_ENABLE_TESTING
    ON
    CACHE
    INTERNAL
    "force dependencies's `HYPERION_ENABLE_TESTING` to the value of this top-level project")
set(HYPERION_ENABLE_TRACY
    ${HYPERION_ENABLE_TRACY}
    CACHE
    INTERNAL
    "force dependencies's `HYPERION_ENABLE_TRACY` to the value of this top-level project")
set(HYPERION_USE_FETCH_CONTENT
    ${HYPERION_USE_FETCH_CONTENT}
    CACHE
    INTERNAL
    "force depedencies's `HYPERION_USE_FETCH_CONTENT` to the value of this top-level project")

if(HYPERION_USE_FETCH_CONTENT)
    include(FetchContent)
endif()

if(${HYPERION_USE_FETCH_CONTENT})
    FetchContent_Declare(hyperion_platform
        GIT_REPOSITORY "https://github.com/braxtons12/hyperion_platform"
        GIT_TAG "v0.2.4"
        )
    FetchContent_MakeAvailable(hyperion_platform)

    FetchContent_Declare(hyperion_mpl
        GIT_REPOSITORY "https://github.com/braxtons12/hyperion_mpl"
        GIT_TAG "v0.7.2"
        )
    FetchContent_MakeAvailable(hyperion_mpl)

    FetchContent_Declare(doctest
        GIT_REPOSITORY "https://github.com/doctest/doctest"
        GIT_TAG "v2.4.11"
        )
    FetchContent_MakeAvailable(doctest)

    FetchContent_Declare(fmt
        GIT_REPOSITORY "https://github.com/fmtlib/fmt"
        GIT_TAG "10.2.1"
        )
    FetchContent_MakeAvailable(fmt)

    SET(BOOST_ENABLE_CMAKE ON)
    FetchContent_Declare(Boost
        GIT_REPOSITORY "https://github.com/boostorg/boost"
        GIT_TAG "boost-1.84.0"
        GIT_SUBMODULES
        libs/assert
        libs/config
        libs/container_hash
        libs/core
        libs/describe
        libs/mp11
        libs/predef
        libs/stacktrace
        libs/static_assert
        libs/throw_exception
        libs/winapi
        tools/cmake
        )
    FetchContent_MakeAvailable(Boost)
else()
    find_package(hyperion_platform REQUIRED)
    find_package(hyperion_mpl REQUIRED)
    find_package(doctest REQUIRED)
    find_package(fmt REQUIRED)
    find_package(Boost COMPONENTS stacktrace REQUIRED)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/hyperion_compiler_settings.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/hyperion_enable_warnings.cmake)

set(HYPERION_MPL_INCLUDE_DIRECTORIES
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
set(HYPERION_MPL_INCLUDE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/include/hyperion"
)
set(HYPERION_MPL_HEADERS
    "${HYPERION_MPL_INCLUDE_PATH}/assert.h"
    "${HYPERION_MPL_INCLUDE_PATH}/source_location.h"
    "${HYPERION_MPL_INCLUDE_PATH}/assert/backtrace.h"
    "${HYPERION_MPL_INCLUDE_PATH}/assert/panic.h"
)

add_library(hyperion_assert STATIC)
add_library(hyperion::assert ALIAS hyperion_assert)
target_include_directories(
    hyperion_assert
    PUBLIC
    ${HYPERION_MPL_INCLUDE_DIRECTORIES}
)
target_sources(
    hyperion_assert
    PUBLIC
    ${HYPERION_MPL_HEADERS}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/panic.cpp"
)
target_link_libraries(
    hyperion_assert
    PUBLIC
    hyperion::platform
    hyperion::mpl
    fmt::fmt
)
if(UNIX)
    target_link_libraries(
        hyperion_assert
        PUBLIC
        pthread
    )
endif()

if(UNIX AND NOT (APPLE OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin"))
    target_link_libraries(
        hyperion_assert
        PUBLIC
        atomic
    )
endif()

if(MSVC)
    target_link_libraries(hyperion_assert
        PUBLIC
        Boost::stacktrace_windbg_cached
        ole32
        dbgeng
    )
    target_compile_definitions(hyperion_assert
        PUBLIC
        BOOST_STACKTRACE_USE_WINDBG_CACHED
    )
elseif(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(hyperion_assert
        PUBLIC
        Boost::stacktrace_windbg
        ole32
        dbgeng
    )
    target_compile_definitions(hyperion_assert
        PUBLIC
        BOOST_STACKTRACE_USE_WINDBG
    )
elseif(APPLE OR ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR NOT TARGET Boost::stacktrace_backtrace)
    target_link_libraries(hyperion_assert
        PUBLIC
        Boost::stacktrace_basic
    )
else()
    target_link_libraries(hyperion_assert
        PUBLIC
        Boost::stacktrace_backtrace
        dl
        backtrace
    )
    target_compile_definitions(hyperion_assert
        PUBLIC
        BOOST_STACKTRACE_USE_BACKTRACE
    )
endif()
target_compile_definitions(hyperion_assert
    PUBLIC
    BOOST_STACKTRACE_LINK
)

hyperion_compile_settings(hyperion_assert)
hyperion_enable_warnings(hyperion_assert)

add_executable(hyperion_assert_main ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(hyperion_assert_main
    PRIVATE
    hyperion::assert
)

hyperion_compile_settings(hyperion_assert_main)
hyperion_enable_warnings(hyperion_assert_main)

add_test(NAME hyperion_assert_main
         COMMAND hyperion_assert_main)

add_executable(hyperion_assert_tests ${CMAKE_CURRENT_SOURCE_DIR}/src/test_main.cpp)
target_link_libraries(hyperion_assert_tests
    PRIVATE
    hyperion::assert
)

hyperion_compile_settings(hyperion_assert_tests)
hyperion_enable_warnings(hyperion_assert_tests)

add_test(NAME hyperion_assert_tests
         COMMAND hyperion_assert_tests)

set(HYPERION_MPL_DOXYGEN_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs/_build/html")
set(HYPERION_MPL_DOXYGEN_HTML "${HYPERION_MPL_DOXYGEN_OUTPUT_DIR}/index.html")

set(HYPERION_MPL_DOCS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/docs")
set(HYPERION_MPL_DOCS_FILES
    "${HYPERION_MPL_DOCS_DIR}/index.rst"
    "${HYPERION_MPL_DOCS_DIR}/quick_start.rst"
)

add_custom_command(
    OUTPUT ${HYPERION_MPL_DOXYGEN_HTML}
    DEPENDS ${HYPERION_MPL_HEADERS} ${DOCS_FILES}
    COMMAND sphix-build -M html . _build
    WORKING_DIRECTORY ${HYPERION_MPL_DOCS_DIR}
    MAIN_DEPENDENCY
    COMMENT "Generating documentation with doxygen, sphinx, and breathe"
    VERBATIM
)

add_custom_target(hyperion_assert_doxygen DEPENDS ${DOXYGEN_HTML})

include(GNUInstallDirs)
install(DIRECTORY ${HYPERION_MPL_DOXYGEN_OUTPUT_DIR} DESTINATION ${CMAKE_INSTALL_DOCDIR})
install(DIRECTORY include/ DESTINATION include)
install(TARGETS hyperion_assert DESTINATION lib)
