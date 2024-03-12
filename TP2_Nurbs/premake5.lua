project 'TP2_Nurbs'
    kind 'ConsoleApp'
    language 'C++'
    cppdialect 'C++17'
    staticruntime 'Off'

    -- on post build copy the executable to the root folder of the project
    postbuildcommands
    {
        'cp %{cfg.buildtarget.relpath} "./"'
    }

    defines 'PROJECT_PATH="%{wks.location}/TP2_Nurbs"'

    files
    {

        "TP2_Nurbs.cpp",

        'src/**.h',
        'src/**.cpp',

        "%{wks.location}/SmartGL/includes/stb_image/**.h",
        "%{wks.location}/SmartGL/includes/stb_image/**.cpp",
    }

    includedirs
    {
        "src",
        "%{wks.location}/TP1_Nurbs/src",
    }

    externalincludedirs
    {
        "%{wks.location}/SmartGL/src",
        "%{wks.location}/SmartGL/includes",
        '%{Includes["glfw"]}',
        '%{Includes["glad"]}',
        '%{Includes["imgui"]}',
        '%{Includes["glm"]}',
        '%{Includes["stb_image"]}',
        '%{Includes["spdlog"]}',
    }

    filter {"action:vs*", "system:windows"}
		targetdir ("%{wks.location}/build/bin/windows/vs/" .. outputdir .. "/%{prj.name}")
		objdir ("%{wks.location}/build/bin-int/windows/vs/" .. outputdir .. "/%{prj.name}")
        links { 'SmartGL' }
        defines 'COMPILER_MSVC'

	filter {"action:gmake*", "system:windows"}
		targetdir ("%{wks.location}/build/bin/windows/mingw/" .. outputdir .. "/%{prj.name}")
		objdir ("%{wks.location}/build/bin-int/windows/mingw/" .. outputdir .. "/%{prj.name}")
        links { 'SmartGL', 'glad', 'glfw', 'imgui', 'gdi32', 'user32', 'shell32' }
        defines 'COMPILER_MINGW'

	filter "system:linux"
		targetdir ("%{wks.location}/build/bin/linux/" .. outputdir .. "/%{prj.name}")
		objdir ("%{wks.location}/build/bin-int/linux/" .. outputdir .. "/%{prj.name}")
        links { 'SmartGL', 'imgui', 'glad', 'glfw', 'dl', 'GL', 'pthread', 'X11', 'Xrandr', 'Xi', 'Xxf86vm', 'Xinerama', 'Xcursor' }
        defines 'COMPILER_GCC'

    filter 'configurations:Debug'
        defines 'DEBUG'
        runtime 'Debug'
        symbols 'On'

    filter 'configurations:Release'
        defines 'RELEASE'
        runtime 'Release'
        optimize 'On'

    filter 'configurations:Dist'
        defines 'DIST'
        runtime 'Release'
        optimize 'On'
