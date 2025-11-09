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
    staticruntime "On"

    targetdir ("Bin/"..outputdir.."/%{prj.name}")
    objdir ("Bin-Intermediate/"..outputdir.."/%{prj.name}")

    pchheader ("PCH.h")
    pchsource ("%{prj.name}/Src/PCH/PCH.cpp")

    characterset ("MBCS")

    -- add dxc lib dir
    libdirs 
	{ 
		"%{wks.location}/%{prj.name}/ThirdParty/dxc/lib/x64",
	}
    
    files
    {
        "%{prj.name}/Src/**.h",
        "%{prj.name}/Src/**.cpp",
        "%{prj.name}/ThirdParty/**.h",
        "%{prj.name}/ThirdParty/**.cpp",
        "%{prj.name}/Src/Shaders/**.hlsl"
    }

    includedirs
    {
		"%{prj.name}/ThirdParty",
		"%{prj.name}/ThirdParty/AgilitySDK/include",
		"%{prj.name}/ThirdParty/AgilitySDK/include/d3dx12",
        "%{prj.name}/Src/PCH",
        "%{prj.name}/Src",
    }

    links
    {
        "dxcompiler"
    }

    nuget { "WinPixEventRuntime:1.0.231030001" }

    filter { "system:windows" }
        prebuildcommands { "powershell -ExecutionPolicy Bypass -File %{wks.location}/Vendor/GetDXC/GetDXC.ps1 %{wks.location}/%{prj.name}/ThirdParty/dxc" }

    -- shader config
    shadermodel ("6.0")
    shaderassembler("AssemblyCode")

    -- used as includes
    filter "files:**.hlsli"
        flags("ExcludeFromBuild")

    filter "files:**.hlsl"
        flags("ExcludeFromBuild")

    filter "system:windows"
        systemversion "latest"

    defines{"DASH_PLATFORM_WINDOWS"}
    defines { 'ENGINE_PATH="' .. path.join(path.getabsolute(""), "%{prj.name}") .. '"' }

    filter "configurations:Debug"
        defines "DASH_DEBUG"
        symbols "On"
        libdirs { "%{wks.location}/%{prj.name}/ThirdParty/assimp/lib/Debug" }
        links { "assimp-vc143-mtd" }
        debugformat "c7"
        flags("NoIncrementalLink")
        flags("NoRuntimeChecks")
        sanitize { "Address" }
        debugenvs { "PATH=$(VC_ExecutablePath_x64);" }

    filter "configurations:Release"
        defines "DASH_RELEASE"
        optimize "On"
        libdirs { "%{wks.location}/%{prj.name}/ThirdParty/assimp/lib/Release" }
        links { "assimp-vc143-mt" }

    filter "configurations:Distribution"
        defines "DASH_DISTRIBUTION"
        optimize "On"
        libdirs { "%{wks.location}/%{prj.name}/ThirdParty/assimp/lib/Release" }
        links { "assimp-vc143-mt" }

project "DashProject"
    location "DashProject"
    kind "WindowedApp"
    language "C++"
    cppdialect "C++20"
    cdialect "C17"
    staticruntime "On"

    targetdir ("Bin/"..outputdir.."/%{prj.name}")
    objdir ("Bin-Intermediate/"..outputdir.."/%{prj.name}")

    characterset ("MBCS")

    --copy dxc dll
    prebuildcommands { "xcopy /Y /D %{wks.location}\\DashCore\\ThirdParty\\dxc\\bin\\x64\\dxcompiler.dll %{wks.location}\\Bin\\"..outputdir.."\\%{prj.name}\\" }
    prebuildcommands { "xcopy /Y /D %{wks.location}\\DashCore\\ThirdParty\\dxc\\bin\\x64\\dxil.dll %{wks.location}\\Bin\\"..outputdir.."\\%{prj.name}\\" }

    prebuildcommands { "xcopy /Y /D %{wks.location}\\DashCore\\ThirdParty\\assimp\\bin\\Debug\\assimp-vc143-mtd.dll %{wks.location}\\Bin\\"..outputdir.."\\%{prj.name}\\" }
    prebuildcommands { "xcopy /Y /D %{wks.location}\\DashCore\\ThirdParty\\assimp\\bin\\Release\\assimp-vc143-mt.dll %{wks.location}\\Bin\\"..outputdir.."\\%{prj.name}\\" }
	
	prebuildcommands { "xcopy /Y /D %{wks.location}\\DashCore\\ThirdParty\\AgilitySDK\\bin\\x64\\D3D12Core.dll %{wks.location}\\Bin\\"..outputdir.."\\%{prj.name}\\" }
	prebuildcommands { "xcopy /Y /D %{wks.location}\\DashCore\\ThirdParty\\AgilitySDK\\bin\\x64\\d3d12SDKLayers.dll %{wks.location}\\Bin\\"..outputdir.."\\%{prj.name}\\" }
	prebuildcommands { "xcopy /Y /D %{wks.location}\\DashCore\\ThirdParty\\AgilitySDK\\bin\\x64\\D3D12StateObjectCompiler.dll %{wks.location}\\Bin\\"..outputdir.."\\%{prj.name}\\" }

    files
    {
        "%{prj.name}/Src/**.h",
        "%{prj.name}/Src/**.cpp"
    }

    includedirs
    {
	    "DashCore/ThirdParty",
		"DashCore/ThirdParty/AgilitySDK/include",
		"DashCore/ThirdParty/AgilitySDK/include/d3dx12",
        "DashCore/Src",
    }

    links
    {
        "DashCore"
    }

    nuget { "WinPixEventRuntime:1.0.231030001" }

    filter "system:windows"
        systemversion "latest"

    defines{"DASH_PLATFORM_WINDOWS"}
    defines { 'PROJECT_PATH="' .. path.join(path.getabsolute(""), "%{prj.name}") .. '"' }

    filter "configurations:Debug"
        defines "DASH_DEBUG"
        symbols "On"
        debugformat "c7"
        flags("NoIncrementalLink")
        flags("NoRuntimeChecks")
        sanitize { "Address" }
        debugenvs { "PATH=$(VC_ExecutablePath_x64);" }

    filter "configurations:Release"
        defines "DASH_RELEASE"
        optimize "On"

    filter "configurations:Distribution"
        defines "DASH_DISTRIBUTION"
        optimize "On"