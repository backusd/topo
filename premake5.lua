workspace "Topo"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Topo"
	location "Topo"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")	
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "Topo/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src"
	}

	defines
	{
		"DIRECTX12"
	}

	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"TOPO_PLATFORM_WINDOWS",
			"TOPO_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines 
		{
			"TOPO_DEBUG",
			"TOPO_ENABLE_ASSERTS"
		}
		symbols "On"

	filter "configurations:Release"
		defines "TOPO_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "TOPO_DIST"
		optimize "On"



project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")	
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Topo/src"
	}

	links
	{
		"Topo"
	}

	defines
	{
		"DIRECTX12"
	}
	
	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"TOPO_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "TOPO_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "TOPO_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "TOPO_DIST"
		optimize "On"