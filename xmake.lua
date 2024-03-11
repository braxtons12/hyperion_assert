---@diagnostic disable: undefined-global,undefined-field
set_project("hyperion_assert")
set_version("0.1.0")

set_xmakever("2.8.7")

set_languages("cxx20")

add_rules("mode.debug", "mode.release")
add_moduledirs("xmake")
add_repositories("hyperion https://github.com/braxtons12/hyperion_packages.git")

option("hyperion_enable_tracy", function()
    set_default(false)
end)

add_requires("hyperion_platform", {
    system = false,
    external = true,
    configs = {
        languages = "cxx20",
        hyperion_enable_tracy = has_config("hyperion_enable_tracy"),
    }
})
add_requires("hyperion_mpl", {
    system = false,
    external = true,
    configs = {
        languages = "cxx20",
        hyperion_enable_tracy = has_config("hyperion_enable_tracy"),
    }
})
add_requires("boost_ut", {
    system = false,
    external = true,
    configs = {
        languages = "cxx20",
    }
})
add_requires("fmt", {
    system = false,
    external = true,
    configs = {
        languages = "cxx20",
    }
})
add_requires("range-v3", {
    system = false,
    external = true,
    configs = {
        languages = "cxx20",
    }
})


if not is_plat("windows") then
    add_requires("libbacktrace", {
        system = false,
        external = true,
    })
end

add_requires("boost", {
    system = false,
    external = true,
    configs = {
        languages = "cxx20",
        stacktrace = true,
    }
})

local hyperion_assert_main_headers = {
    "$(projectdir)/include/hyperion/assert.h",
    "$(projectdir)/include/hyperion/source_location.h",
}
local hyperion_assert_headers = {
    "$(projectdir)/include/hyperion/assert/backtrace.h",
    "$(projectdir)/include/hyperion/assert/panic.h",
}
local hyperion_assert_detail_headers = {
    "$(projectdir)/include/hyperion/assert/detail/lexer.h",
}
local hyperion_assert_sources = {
    "$(projectdir)/src/assert/panic.cpp",
    "$(projectdir)/src/assert/detail/lexer.cpp",
}

local setup_boost_config = function(target)
    if target:is_plat("windows") then
        local stacktrace_lib = "libboost_stacktrace_windbg"
        if target:has_tool("cxx", "gcc", "clang") then
            target:add("defines", "BOOST_STACKTRACE_USE_WINDBG", { public = true })
        else
            target:add("defines", "BOOST_STACKTRACE_USE_WINDBG_CACHED", { public = true })
            stacktrace_lib = stacktrace_lib .. "_cached"
        end

        if target:get("config", "multi") then
            stacktrace_lib = stacktrace_lib .. "-mt"
        end

        local no_runtime_set = not target:has_runtime("MT")
                           and not target:has_runtime("MTd")
                           and not target:has_runtime("MD")
                           and not target:has_runtime("MDd")
        if target:has_runtime("MT") or no_runtime_set then
            stacktrace_lib = stacktrace_lib .. "-s"
        elseif target:has_runtime("MTd") then
            stacktrace_lib = stacktrace_lib .. "-sgd"
        elseif target:has_runtime("MDd") then
            stacktrace_lib = stacktrace_lib .. "-gd"
        end

        target:add("links", stacktrace_lib, {public = true})
        target:add("links", "ole32", "dbgeng", { public = true })
    else
        target:add("defines", "BOOST_STACKTRACE_USE_BACKTRACE", { public = true })
        target:add("links", "dl", { public = true })
    end
    target:add("defines", "BOOST_STACKTRACE_LINK", { public = true })
end

target("hyperion_assert", function()
    set_kind("static")
    set_languages("cxx20")
    set_default(true)

    add_includedirs("$(projectdir)/include", { public = true })
    add_headerfiles(hyperion_assert_main_headers, { prefixdir = "hyperion", public = true })
    add_headerfiles(hyperion_assert_headers, { prefixdir = "hyperion/assert", public = true })
    add_headerfiles(hyperion_assert_detail_headers, { prefixdir = "hyperion/assert/detail", public = true })
    add_files(hyperion_assert_sources)

    on_config(function(target)
        import("hyperion_compiler_settings", { alias = "settings" })
        settings.set_compiler_settings(target)
        setup_boost_config(target)
    end)

    add_options("hyperion_enable_tracy", { public = true })

    add_packages("hyperion_platform", "hyperion_mpl", "boost", "fmt", "range-v3", { public = true })
    if not is_plat("windows") then
        add_packages("libbacktrace", { public = true })
        add_links("pthread", { public = true })
        if not is_plat("macosx") then
            add_links("atomic", { public = true })
        end
    end
end)

target("hyperion_assert_main", function()
    set_kind("binary")
    set_languages("cxx20")
    set_default(true)

    add_files("$(projectdir)/src/main.cpp")

    add_deps("hyperion_assert")

    on_config(function(target)
        import("hyperion_compiler_settings", { alias = "settings" })
        settings.set_compiler_settings(target)
    end)
    add_tests("hyperion_assert_main")
end)

target("hyperion_assert_tests", function()
    set_kind("binary")
    set_languages("cxx20")
    set_default(true)

    add_files("$(projectdir)/src/test_main.cpp")
    add_defines("HYPERION_ENABLE_TESTING=1")

    add_deps("hyperion_assert")
    add_packages("boost_ut")

    on_config(function(target)
        import("hyperion_compiler_settings", { alias = "settings" })
        settings.set_compiler_settings(target)
    end)

    add_tests("hyperion_assert_tests")
end)

target("hyperion_assert_docs", function()
    set_kind("phony")
    set_default(false)
    on_build(function(_)
        local old_dir = os.curdir()
        os.cd("$(projectdir)/docs")
        os.exec("sphinx-build -M html . _build")
        os.cd(old_dir)
    end)
end)
