add_repositories("package_repo_singleton https://github.com/MasterLaplace/Singleton.git")

add_rules("mode.debug", "mode.release", "plugin.vsxmake.autoupdate")

option("with-autoupdate")
    set_default(true)
    set_showmenu(true)
    set_description("Enable automatic game download/update feature")
option_end()

option("pack-server")
    set_default(true)
    set_showmenu(true)
    set_description("Include server in package")
option_end()

option("pack-client")
    set_default(true)
    set_showmenu(true)
    set_description("Include client library in package")
option_end()

add_requires("nlohmann_json", "singleton")

if has_config("with-autoupdate") then
    add_requires("libcurl", {configs = {openssl3 = is_plat("linux", "macosx")}})
    add_requires("libgit2", {configs = {https = is_plat("windows") and "winhttp" or "openssl3", tools = false}})
end

includes("@builtin/xpack")
includes("examples")

set_project("Flakkari")
set_license("MIT")

target("flakkari-server")
    set_kind("binary")
    set_default(true)
    set_languages("cxx20")
    set_policy("build.warning", true)
    set_version("0.9.0")

    add_packages("nlohmann_json", "singleton")

    if has_config("with-autoupdate") then
        add_packages("libcurl", "libgit2")
        add_defines("FLAKKARI_AUTO_UPDATE")
        set_policy("check.target_package_licenses", false)
    end

    if is_mode("debug") then
        add_defines("_DEBUG")
        set_symbols("debug")
        set_optimize("none")
    elseif is_mode("release") then
        add_defines("NDEBUG")
        set_optimize("fastest")
    end

    add_files("Flakkari/**.cpp")

    remove_files("Flakkari/Client/**.cpp")

    if not has_config("with-autoupdate") then
        remove_files("Flakkari/Server/Internals/GameDownloader.cpp")
    end

    add_headerfiles("Flakkari/**.h", { public = true })
    add_headerfiles("Flakkari/**.hpp", { public = true })

    remove_headerfiles("Flakkari/Client/**.hpp")

    add_includedirs("Flakkari/", { public = true })
    add_includedirs("Flakkari/Engine", { public = true })
    add_includedirs("Flakkari/Engine/EntityComponentSystem", { public = true })
    add_includedirs("Flakkari/Engine/EntityComponentSystem/Components", { public = true })
    add_includedirs("Flakkari/Engine/EntityComponentSystem/Systems", { public = true })
    add_includedirs("Flakkari/Engine/Math", { public = true })
    add_includedirs("Flakkari/Logger", { public = true })
    add_includedirs("Flakkari/Network", { public = true })
    add_includedirs("Flakkari/Protocol", { public = true })
    add_includedirs("Flakkari/Server", { public = true })
    add_includedirs("Flakkari/Server/Client", { public = true })
    add_includedirs("Flakkari/Server/Game", { public = true })
    add_includedirs("Flakkari/Server/Internals", { public = true })

    if is_plat("windows") then
        add_syslinks("Iphlpapi")
    end
target_end()

target("flakkari-client")
    set_kind("static")
    set_default(false)
    set_languages("cxx20")
    set_policy("build.warning", true)
    set_version("0.9.0")

    if is_mode("debug") then
        add_defines("_DEBUG")
        set_symbols("debug")
        set_optimize("none")
    elseif is_mode("release") then
        add_defines("NDEBUG")
        set_optimize("fastest")
    end

    add_files("Flakkari/**.cpp")

    remove_files("Flakkari/*.cpp")
    remove_files("Flakkari/Server/**.cpp")
    remove_files("Flakkari/Engine/**.cpp")
    remove_files("Flakkari/Protocol/Engine/**.cpp")

    add_headerfiles("Flakkari/**.h", { public = true })
    add_headerfiles("Flakkari/**.hpp", { public = true })

    remove_headerfiles("Flakkari/Server/**.hpp")
    remove_headerfiles("Flakkari/Engine/**.hpp")
    remove_headerfiles("Flakkari/Protocol/Engine/**.hpp")

    add_includedirs("Flakkari/", { public = true })
    add_includedirs("Flakkari/Logger", { public = true })
    add_includedirs("Flakkari/Network", { public = true })
    add_includedirs("Flakkari/Protocol", { public = true })
target_end()

-- Doc: https://xmake.io/#/manual/xpack
xpack("flakkari")
    set_formats("nsis", "zip", "srczip", "targz", "srctargz", "runself")
    set_title("Flakkari Full v$(version) ($(arch))")
    set_author("MasterLaplace")
    set_company("ME.inc")
    set_description("Flakkari is a UDP/TCP game server and client library initially developed for the R-Type Epitech project and updated for the Video Games course at University Laval.")
    set_homepage("https://github.com/MasterLaplace/Flakkari")
    set_licensefile("LICENSE")
    set_copyright("Copyright (C) 2023-present, Me.inc & MasterLaplace")
    set_version("0.9.0")

    if has_config("pack-server") then
        add_targets("flakkari-server")
    end
    if has_config("pack-client") then
        add_targets("flakkari-client")
    end

    set_iconfile("docs/Images/icon.ico")

    add_sourcefiles("Flakkari/**.cpp")
    add_sourcefiles("Flakkari/**.hpp")
    add_sourcefiles("Flakkari/**.h")
    add_installfiles("Flakkari/**.h", {prefixdir = "include"})
    add_installfiles("Flakkari/**.hpp", {prefixdir = "include"})
    add_installfiles("doc/*.md", {prefixdir = "share/docs"})
    add_installfiles("*.md", {prefixdir = "share/docs"})

    on_load(function (package)
        local pack_server = has_config("pack-server")
        local pack_client = has_config("pack-client")
        local pack_type = ""

        if pack_server and pack_client then
            pack_type = "full"
        elseif pack_server then
            pack_type = "server"
        elseif pack_client then
            pack_type = "client"
        end

        if package:from_source() then
            package:set("basename", "flakkari-" .. pack_type .. "-$(plat)-src-v$(version)")
        else
            package:set("basename", "flakkari-" .. pack_type .. "-$(plat)-$(arch)-v$(version)")
        end

        local format = package:format()
        if format == "srpm" then
            package:add("buildrequires", "make")
            package:add("buildrequires", "gcc")
            package:add("buildrequires", "gcc-c++")
        end
    end)

    after_installcmd(function (package, batchcmds)
        if not has_config("pack-server") then
            return
        end

        batchcmds:mkdir(package:installdir("Games"))
        batchcmds:cp("Games/*", package:installdir("Games"), {rootdir = "."})

        local games_dir = package:installdir("Games")

        if is_plat("windows") then
            batchcmds:runv(string.format('setx FLAKKARI_INSTALL_GAME_DIR "%s"', games_dir))
        elseif is_plat("linux") or is_plat("macosx") then
            local shell_config = os.getenv("SHELL") == "/bin/zsh" and "~/.zshrc" or "~/.bashrc"
            batchcmds:runv(string.format(
                'echo \'export FLAKKARI_INSTALL_GAME_DIR="%s"\' >> %s',
                games_dir, shell_config
            ))
        else
            batchcmds:showmsg("Please set the environment variable FLAKKARI_INSTALL_GAME_DIR manually to: " .. games_dir)
        end
    end)

    after_uninstallcmd(function (package, batchcmds)
        if not has_config("pack-server") then
            return
        end
        batchcmds:rmdir(package:installdir("Games"))

        if is_plat("windows") then
            batchcmds:runv('reg delete "HKCU\\Environment" /F /V FLAKKARI_INSTALL_GAME_DIR')
        elseif is_plat("linux") or is_plat("macosx") then
            local shell_config = os.getenv("SHELL") == "/bin/zsh" and "~/.zshrc" or "~/.bashrc"
            batchcmds:runv(string.format(
                'sed -i "/export FLAKKARI_INSTALL_GAME_DIR=.*/d" %s',
                shell_config
            ))
        else
            batchcmds:showmsg("Please remove the environment variable FLAKKARI_INSTALL_GAME_DIR manually if it was set.")
        end
    end)
xpack_end()
