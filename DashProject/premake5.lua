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

    -- add dxc lib dir
    libdirs { "%{wks.location}/%{prj.name}/Src/ThirdParty/dxc/lib/x64" }
    libdirs { "%{wks.location}/%{prj.name}/Src/ThirdParty/assimp/lib/x64" }

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
        "%{prj.name}/Src/ThirdParty",
    }

    links
    {
        "dxcompiler.lib",
        "assimp-vc143-mtd.lib"
    }

    defines { 'ENGINE_PATH="' .. path.join(path.getabsolute(""), "%{prj.name}") .. '"' }

    -- shader config
    shadermodel ("6.0")
    shaderassembler("AssemblyCode")

    --nuget { "Assimp:3.0.0", "Assimp.redist:3.0.0" }

    filter { "system:windows" }
        prebuildcommands { "powershell -ExecutionPolicy Bypass -File %{wks.location}/Vendor/GetDXC/GetDXC.ps1 %{wks.location}/%{prj.name}/Src/ThirdParty/dxc" }

    -- used as includes
    filter "files:**.hlsli"
        flags("ExcludeFromBuild")
    
    filter "files:**.hlsl"
        flags("ExcludeFromBuild")

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

    defines { 'PROJECT_PATH="' .. path.join(path.getabsolute(""), "%{prj.name}") .. '"' }

    --copy dxc dll
    prebuildcommands { "xcopy /Y /D %{wks.location}\\DashCore\\Src\\ThirdParty\\dxc\\bin\\x64\\dxcompiler.dll %{wks.location}\\Bin\\"..outputdir.."\\%{prj.name}\\" }
    prebuildcommands { "xcopy /Y /D %{wks.location}\\DashCore\\Src\\ThirdParty\\dxc\\bin\\x64\\dxil.dll %{wks.location}\\Bin\\"..outputdir.."\\%{prj.name}\\" }

    --prebuildcommands { "xcopy /Y /D %{wks.location}\\DashCore\\Src\\ThirdParty\\assimp\\lib\\x64\\assimp-vc143-mtd.dll %{wks.location}\\Bin\\"..outputdir.."\\%{prj.name}\\" }

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