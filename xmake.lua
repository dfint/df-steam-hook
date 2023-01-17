-- project
set_project("dfint-hook")
set_languages("cxxlatest")

-- allowed
set_allowedarchs("windows|x64")
set_allowedmodes("debug", "release")
-- default
set_defaultarchs("windows|x64")
set_defaultmode("release")

-- rules
add_rules("plugin.vsxmake.autoupdate")
add_rules("mode.release")

-- deps
--3rd party local libs
add_linkdirs("deps/lib")
add_includedirs("deps/include")
--3rd party remote libs
add_requires("spdlog")
add_requires("vcpkg::detours")
add_requires("toml++")

option("hook_version")
    set_default("not-defined")

target("dfint_hook")
    set_default(true)
    set_kind("shared")
    set_basename("dfint_hook")
    set_targetdir("D:\\Games\\Dwarf Fortress 50.05\\dfint_data") -- build to DF dir, handy for testing
    set_pcxxheader("src/hook/pch.h")
    add_files("src/hook/*.cpp")
    add_packages("spdlog", "vcpkg::detours", "toml++")
    add_defines("HOOK_VERSION=\"$(hook_version)\"")

target("dfint_launcher")
    set_default(true)
    set_kind("binary")
    set_basename("dfint_launcher")
    set_targetdir("D:\\Games\\Dwarf Fortress 50.05\\") -- build to DF dir, handy for testing
    add_files("src/launcher/*.cpp")
    add_packages("vcpkg::detours")
