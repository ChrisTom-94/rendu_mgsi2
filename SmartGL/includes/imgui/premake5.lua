project "imgui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	systemversion "latest"
	staticruntime "on"

	files
	{
		"imconfig.h",
		"imgui.h",
		"imgui.cpp",
		"imgui_draw.cpp",
		"imgui_internal.h",
		"imgui_widgets.cpp",
		"imstb_rectpack.h",
		"imstb_textedit.h",
		"imstb_truetype.h",
		-- "imgui_demo.cpp",
		"imgui_tables.cpp",
		"backends/imgui_impl_glfw.h",
		"backends/imgui_impl_glfw.cpp",
		"backends/imgui_impl_opengl3.h",
		"backends/imgui_impl_opengl3.cpp",
	}

	includedirs
	{
		"./",
		"%{Includes['glfw']}",
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

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
