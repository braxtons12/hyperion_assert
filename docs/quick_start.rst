Quick Start Guide
*****************

hyperion::assert supports both CMake and XMake, and incorporating it in your project is quick and
easy.

CMake
-----

hyperion::assert is easily incorporated into a raw CMake project with :cmake:`FetchContent` or
other methods like :cmake:`add_subdirectory`\. Example for :cmake:`FetchContent`\:

.. code-block:: cmake
    :caption: CMakeLists.txt
    :linenos:

    # Include FetchContent so we can use it
    include(FetchContent)

    # Declare the dependency on hyperion-utils and make it available for use
    FetchContent_Declare(hyperion_assert
        GIT_REPOSITORY "https://github.com/braxtons12/hyperion_assert"
        GIT_TAG "v0.1.0")
    FetchContent_MakeAvailable(hyperion_assert)

    # For this example, we create an executable target and link hyperion::assert to it
    add_executable(MyExecutable "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
    target_link_libraries(MyExecutable PRIVATE hyperion::assert)

Note that hyperion::assert depends on
`hyperion::platform <https://github.com/braxtons12/hyperion_platform>`_\, for platform and feature
detection macros and other core utilities,
`hyperion::mpl <https://github.com/braxtons12/hyperion_mpl>`_\, for metaprogramming,
`boost::stacktrace <https://github.com/boostorg/stacktrace>`_\, for stacktrace support,
`fmtlib <https://github.com/fmtlib/fmt>`_\, for string formatting,
and `doctest <https://github.com/doctest/doctest>`_\, for testing.
On operating systems other than Windows and MacOS, hyperion::assert additionally depends on
`libbacktrace <https://github.com/ianlancetaylor/libbacktrace>`_ for backtrace support.
The hyperion libraries used as dependencies havee several optional features with optional
dependencies, and hyperion::assert exposes those settings to its users as well (see the relevant
documentation, e.g. the
`hyperion::platform documentation <https://braxtons12.github.io/hyperion_platform/quick_start.html>`_
for how to enable these).

By default, :cmake:`FetchContent` will be used to obtain these dependencies, but you can disable
this by setting :cmake:`HYPERION_USE_FETCH_CONTENT` to :cmake:`OFF`\, in which case you will need to
make sure each package is findable via CMake's :cmake:`find_package`\.

XMake
-----

XMake is a new(er) Lua-based build system with integrated package management. It is the preferred
way to use Hyperion packages. Example:

.. code-block:: lua
    :caption: xmake.lua
    :linenos:

    set_project("my_project")

    -- add the hyperion_packages git repository as an XMake package repository/registry
    add_repositories("hyperion https://github.com/braxtons12/hyperion_packages.git")

    -- add hyperion_assert as a required dependency for the project
    add_requires("hyperion_assert", {
        system = false,
        external = true,
    })
    
    -- For this example, we create an executable target and link hyperion::assert to it
    target("my_executable")
        set_kind("binary")
        add_packages("hyperion_assert")

Note that with XMake, hyperion::assert requires the same dependencies as with the CMake build system,
barring one exception. That exception is that with XMake, hyperion::assert will depend on
libbacktrace on all non-Windows operating systems, including MacOS. This is because XMake provides
a package for libbtrace, which allows us to extend its usage to operating systems that may not have
libbacktrace installed at the system level, like MacOS.
Third-party dependencies will be pulled from xmake-repo, the package repository/registry for XMake,
and dependencies on other hyperion libraries will be pulled from github via the 
`hyperion package repository/registry for xmake <https://github.com/braxtons12/hyperion_packages>`_\.

