set_project("BurdenToBearRestored")
set_version("1.0")
set_plat("windows")
set_toolchains("msvc")
set_languages("cxxlatest")
set_warnings("all")
set_optimize("faster")
set_symbols("debug")
set_policy("build.optimization.lto", true)
add_includedirs("../")

target("BurdenToBearRestored")
    set_arch("x86")
    set_kind("shared")
    add_files(
        "../shared/NVSEManager/NVSEGlobalManager.cpp",
        "../shared/SafeWrite/SafeWrite.cpp",
        "*.cpp"
    )
    add_links("user32")
    add_defines(
        "WIN32",
        "_WINDOWS",
        "RUNTIME_VERSION=0x040020D0",
        "RUNTIME",
        "_CRT_SECURE_NO_WARNINGS"
    )
target_end()
