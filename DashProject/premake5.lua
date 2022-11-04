workspace "DashProject"
    architecture "x64"

    configurations
    {
        "Debug",
        "Release",
        "Distribution"
    }

    flags
	{
        --多核并行编译
		"MultiProcessorCompile" 
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
        "%{prj.name}/Src/**.cpp",
        "%{prj.name}/Src/**.hlsl"
    }

    includedirs
    {
        "%{prj.name}/Src/PCH",
        "%{prj.name}/Src",
    }

    -- shader config
    shadermodel ("6.0")
    shaderassembler("AssemblyCode")
    local shaderdir = "%{prj.name}/Src/Shaders/"
    print(shaderdir)

    -- used as includes
    filter "files:**.hlsli"
        flags("ExcludeFromBuild")
    
    filter "files:**.hlsl"
        flags("ExcludeFromBuild")
        --shaderobjectfileoutput(shaderdir.."%{file.basename}"..".cso")
        --shaderassembleroutput(shaderdir.."%{file.basename}"..".asm")

    --[[
    filter "files:**_PS.hlsl"
        shadertype("Pixel")
        shaderentry ("PSMain")
        shaderoptions("-T \"ps_6_0\"")
        shaderoptions("/WX")
        --shaderoptions("/Zi")
        shaderoptions("/Zp")
        shaderoptions("-Qstrip_debug")
        shaderoptions("-Qstrip_reflect")
        
    
    filter "files:**_VS.hlsl"
        shadertype("Vertex")
        shaderentry ("VSMain")
        shaderoptions("/WX")
    ]]

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