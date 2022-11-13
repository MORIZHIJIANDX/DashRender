#include "PCH.h"
#include "ShaderCompiler.h"
#include "Utility/StringUtility.h"

namespace Dash
{
	using namespace Microsoft::WRL;

	void ShaderCompiler::Init()
	{
		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&mUtils));

		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&mCompiler));

		//create default include handler
		mUtils->CreateDefaultIncludeHandler(&mIncludeHandler);
	}

	void ShaderCompiler::CompileShader(const std::string& fileName, const std::string& entryPoint)
	{
		std::vector<LPCWSTR> args;

		// entry point
		args.push_back(L"-E");
		args.push_back(UTF8ToWideString(entryPoint).c_str());

		// prifle
		args.push_back(L"-E");
		args.push_back(UTF8ToWideString(entryPoint).c_str());
	}
}