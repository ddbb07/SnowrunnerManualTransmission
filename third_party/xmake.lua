target("ois")
    set_kind("static")

    add_files("OIS/src/*.cpp", "OIS/src/win32/*.cpp")

    set_configdir("$(builddir)/ois")
    add_configfiles("OIS/includes/OISPrereqs.h.in", {pattern = "@(.-)@"})
    set_configvar("OIS_MAJOR_VERSION", 1)
    set_configvar("OIS_MINOR_VERSION", 6)
    set_configvar("OIS_PATCH_VERSION", 0)
    set_configvar("OIS_SOVERSION", "1.6.0")

    add_includedirs("OIS/includes", "$(builddir)/ois", {public = true})
    add_includedirs("OIS/includes/win32")
    add_defines("OIS_WIN32_XINPUT_SUPPORT")
    add_syslinks("dinput8", "dxguid", "xinput1_4", "ole32", "oleaut32", {public = true})
