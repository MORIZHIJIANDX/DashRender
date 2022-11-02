workspace "DashProject"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Distribution"
    }

    startproject "DashProject"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "DashCore"
    location "DashCore"
    kind "StaticLib"
	language "C++"
	cppdialect "C++20"
    cdialect "C17"
	staticruntime "on"

    targetdir ("Bin/"..outputdir.."/%{prj.name}")
    objdir ("Bin-Intermediate/"..outputdir.."/%{prj.name}")

    pchheader ("PCH.h")
    pchsource ("%{prj.name}/Src/PCH/PCH.cpp")

    characterset ("MBCS")

    files
    {
        "%{prj.name}/Src/**.h",
        "%{prj.name}/Src/**.cpp"
    }

    includedirs
    {
        "%{prj.name}/Src/PCH",
        "%{prj.name}/Src",
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "DASH_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "DASH_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "DASH_RELEASE"
        optimize "On"

    filter "configurations:Distribution"
        defines "DASH_DISTRIBUTION"
        optimize "On"

project "DashProject"
    location "DashProject"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    cdialect "C17"
    staticruntime "on"

    targetdir ("Bin/"..outputdir.."/%{prj.name}")
    objdir ("Bin-Intermediate/"..outputdir.."/%{prj.name}")

    characterset ("MBCS")

    files
    {
        "%{prj.name}/Src/**.h",
        "%{prj.name}/Src/**.cpp"
    }

    includedirs
    {
        "DashCore/Src"
    }

    links
    {
        "DashCore"
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "DASH_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "DASH_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "DASH_RELEASE"
        optimize "On"

    filter "configurations:Distribution"
        defines "DASH_DISTRIBUTION"
        optimize "On"