project 'glad'
    kind 'StaticLib'
    language 'C'
    staticruntime 'On'

    files
    {
        'include/glad/gl.h',
        'include/KHR/khrplatform.h',
        'src/gl.c'
    }

    includedirs
    {
        'include'
    }

    filter {"action:vs*", "system:windows"}
		targetdir ("%{wks.location}/build/bin/windows/vs/" .. outputdir .. "/Dependencies/%{prj.name}")
		objdir ("%{wks.location}/build/bin-int/windows/vs/" .. outputdir .. "/Dependencies/%{prj.name}")

	filter {"action:gmake*", "system:windows"}
		targetdir ("%{wks.location}/build/bin/windows/mingw/" .. outputdir .. "/Dependencies/%{prj.name}")
		objdir ("%{wks.location}/build/bin-int/windows/mingw/" .. outputdir .. "/Dependencies/%{prj.name}")

	filter "system:linux"
		targetdir ("%{wks.location}/build/bin/linux/" .. outputdir .. "/Dependencies/%{prj.name}")
		objdir ("%{wks.location}/build/bin-int/linux/" .. outputdir .. "/Dependencies/%{prj.name}")

    filter 'system:windows'
        systemversion 'latest'
        defines
        {
            'GLFW_INCLUDE_NONE'
        }

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