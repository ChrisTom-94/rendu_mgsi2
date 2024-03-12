project 'SmartGL'
    kind 'StaticLib'
    language 'C++'
    cppdialect 'C++17'
    staticruntime 'Off'
    systemversion 'latest'

    pchsource "src/precompiled.cpp"
    pchheader "precompiled.h"

    files
    {
        'src/**.h',
        'src/**.cpp',

        'includes/glm/glm/**.hpp',
		'includes/glm/glm/**.inl',

        'includes/stb_image/stb_image.h',
        'includes/stb_image/stb_image.cpp',
    }

    includedirs
    {
        'src',
        'includes',
    }

    externalincludedirs
    {
        '%{Includes["glfw"]}',
        '%{Includes["glad"]}',
        '%{Includes["imgui"]}',
        '%{Includes["glm"]}',
        '%{Includes["stb_image"]}',
        '%{Includes["spdlog"]}',
    }

    defines
    {
        'GLFW_INCLUDE_NONE',
        '_CRT_SECURE_NO_WARNINGS'
    }


    filter {"action:vs*", "system:windows"}
        pchheader "precompiled.h"
        targetdir ("%{wks.location}/build/bin/windows/vs/" .. outputdir .. "/%{prj.name}")
        objdir ("%{wks.location}/build/bin-int/windows/vs/" .. outputdir .. "/%{prj.name}")
        links { 'glad', 'glfw', 'imgui', 'opengl32', 'gdi32', 'user32', 'shell32' }
        defines 'SMART_PLATFORM_WINDOWS'

    filter {"action:gmake*", "system:windows"}
        pchheader "precompiled.h"
        targetdir ("%{wks.location}/build/bin/windows/mingw/" .. outputdir .. "/%{prj.name}")
        objdir ("%{wks.location}/build/bin-int/windows/mingw/" .. outputdir .. "/%{prj.name}")
        links { 'glad', 'glfw', 'imgui', 'opengl32', 'gdi32', 'user32', 'shell32' }
        defines {'SMART_PLATFORM_WINDOWS', 'SMART_COMPILER_MINGW'}

    filter "system:linux"
        targetdir ("%{wks.location}/build/bin/linux/" .. outputdir .. "/%{prj.name}")
        objdir ("%{wks.location}/build/bin-int/linux/" .. outputdir .. "/%{prj.name}")
        links {'glad', 'glfw', 'imgui', 'dl', 'GL', 'pthread', 'X11', 'Xrandr', 'Xi', 'Xxf86vm', 'Xinerama', 'Xcursor' }
        defines 'SMART_PLATFORM_LINUX'


    filter 'configurations:Debug'
        defines 'SMART_DEBUG'
        runtime 'Debug'
        symbols 'On'
    filter 'configurations:Release'
        defines 'SMART_RELEASE'
        runtime 'Release'
        optimize 'On'
    filter 'configurations:Dist'
        defines 'SMART_DIST'
        runtime 'Release'
        optimize 'On'
