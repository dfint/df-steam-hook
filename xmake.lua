-- project
set_project("dfint-hook")
set_languages("c++20")

-- allowed
set_allowedarchs("windows|x64")
set_allowedmodes("release")
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

target("dfint_hook")
    set_default(true)
    set_kind("shared")
    set_basename("dfint_hook")
    -- set_targetdir("E:\\Games\\Dwarf Fortress Steam 50.05\\dfint_data") -- build to DF dir, handy for testing
    set_pcxxheader("src/hook/pch.h")
    add_files("src/hook/*.cpp")
    add_packages("spdlog", "vcpkg::detours", "toml++")

target("dfint_launcher")
    set_default(true)
    set_kind("binary")
    set_basename("dfint_launcher")
    -- set_targetdir("E:\\Games\\Dwarf Fortress Steam 50.05\\") -- build to DF dir, handy for testing
    add_files("src/launcher/*.cpp")
    add_packages("vcpkg::detours")

