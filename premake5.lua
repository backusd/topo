workspace "Topo"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

startproject "Sandbox"

project "Topo"
	location "Topo"
	kind "StaticLib"
	language "C++"
	cppdialect "C++latest"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")	
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource "Topo/src/pch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/src/**.hlsl",
		"%{prj.name}/src/**.hlsli"
	}

	includedirs
	{
		"%{prj.name}/src"
	}

	defines
	{
		"TOPO_CORE",
		"DIRECTX12"
	}

	shaderobjectfileoutput ("../bin/" .. outputdir .. "/topo/cso/%%(Filename).cso")

	filter { "files:**.hlsli" }
		flags "ExcludeFromBuild"
	filter { "files:**.hlsl" }
		shadermodel "6.4"
	filter { "files:**-ps.hlsl" }
		shadertype "Pixel"
	filter { "files:**-vs.hlsl" }
		shadertype "Vertex"

	filter { "system:windows", "configurations:Debug" }
		shaderoptions { "/Qembed_debug" }

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"TOPO_PLATFORM_WINDOWS"
		}

		postbuildcommands
		{
			("{RMDIR} ../Sandbox/cso"),
			("{MKDIR} ../Sandbox/cso"),
			("{COPYFILE} ../bin/" .. outputdir .. "/topo/cso/* ../Sandbox/cso")
		}

	filter "configurations:Debug"
		defines 
		{
			"TOPO_DEBUG",
			"TOPO_ENABLE_ASSERTS"
		}
		symbols "on"

	filter "configurations:Release"
		defines "TOPO_RELEASE"
		optimize "on"

	filter "configurations:Dist"
		defines "TOPO_DIST"
		optimize "on"



project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++latest"
	staticruntime "on"

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
		systemversion "latest"
		defines
		{
			"TOPO_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines 
		{
			"TOPO_DEBUG",
			"TOPO_ENABLE_ASSERTS"
		}
		symbols "on"

	filter "configurations:Release"
		defines "TOPO_RELEASE"
		optimize "on"

	filter "configurations:Dist"
		defines "TOPO_DIST"
		optimize "on"