add_rules("mode.release")
set_languages("c++20")

--3rd party local libs
add_linkdirs("deps/lib")
add_includedirs("deps/include")
--3rd party remote libs
add_requires("spdlog")
add_requires("vcpkg::detours")

target("df-hook")
    set_kind("shared")
    set_basename("df-steam-translate-hook")
    -- set_targetdir("E:\\Games\\Dwarf Fortress Steam 50.04\\") -- build to DF dir, handy for testing
    set_pcxxheader("src/pch.h")
    add_files("src/*.cpp")
    add_packages("spdlog")
    add_packages("vcpkg::detours")
