workspace "topo"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "topo"
	location "topo"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")	
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"TOPO_PLATFORM_WINDOWS",
			"TOPO_BUILD_DLL",
			"TOPO_TEST"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/sandbox")
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



project "sandbox"
	location "sandbox"
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
		"topo/src"
	}

	links
	{
		"topo"
	}
	
	filter "system:windows"
		cppdialect "C++latest"
		staticruntime "On"
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