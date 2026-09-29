set_version("1.5")
set_languages("cxx20")
set_plat("mingw")
set_arch("x86_64")
add_rules("mode.debug", "mode.release")
set_toolchains("mingw[clang]@llvm-mingw")

add_requires("llvm-mingw", "microsoft-detours", "stb")
add_requires("imgui v1.92.9+b", {configs = {dx11 = true, win32 = true}})
add_requires("spdlog", {configs = {header_only = true, std_format = true}})

includes("third_party")

target("SMT")
    set_kind("shared")
    set_prefixname("")
    set_extension(".asi")

    add_deps("ois")
    add_packages("microsoft-detours", "imgui", "stb", "spdlog")

    add_files("src/*.cpp", "third_party/kiero/*.cpp")
    add_includedirs("src", "third_party", "third_party/kiero", "third_party/inifile-cpp/include", "third_party/tsl")
    add_defines("IMGUI_DEFINE_MATH_OPERATORS", "NOMINMAX", "UNICODE", "_UNICODE", "WIN32_LEAN_AND_MEAN")
    on_load(function (target)
        target:add("defines", "VERSION=" .. target:version())
    end)
    add_syslinks("d3d11", "d3dcompiler", "dxgi", "gdi32", "dwmapi")
    add_shflags("-static", "-Wl,--exclude-all-symbols")
