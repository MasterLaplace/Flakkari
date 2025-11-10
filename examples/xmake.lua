target("example-client")
    set_kind("binary")
    set_default(false)
    set_languages("cxx20")
    set_policy("build.warning", true)

    add_files("client.cpp")

    add_deps("flakkari-client")

    add_includedirs("$(projectdir)", { public = false })

    if is_mode("debug") then
        add_defines("_DEBUG")
        set_symbols("debug")
        set_optimize("none")
    elseif is_mode("release") then
        add_defines("NDEBUG")
        set_optimize("fastest")
    end

    if is_plat("windows") then
        add_syslinks("ws2_32", "Iphlpapi")
    elseif is_plat("linux") then
        add_syslinks("pthread")
    elseif is_plat("macosx") then
        add_syslinks("pthread")
    end
target_end()
