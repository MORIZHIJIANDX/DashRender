#include "PCH.h"
#include "ShaderCompiler.h"
#include "Utility/StringUtility.h"
#include "Utility/FileUtility.h"
#include "DX12Helper.h"

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

	ComPtr<IDxcBlob> ShaderCompiler::CompileShader(const std::string& fileName, const std::string& entryPoint, const std::vector<std::string>& defines)
	{
		if (!FileUtility::IsPathExistent(fileName))
		{
			LOG_WARNING << "Cannot found file : " << fileName;
			return nullptr;
		}

		std::wstring wFileName = UTF8ToWideString(fileName);
		
		LOG_INFO << "Load File : " << fileName;

		ComPtr<IDxcBlobEncoding> source = nullptr;
		DX_CALL(mUtils->LoadFile(wFileName.c_str(), nullptr, &source));

		std::vector<LPCWSTR> args;

		// Optional shader source file name for error reporting
		args.push_back(wFileName.c_str());

		// Entry point
		std::wstring wEntryPoint = UTF8ToWideString(entryPoint);

		args.push_back(L"-E");
		args.push_back(wEntryPoint.c_str());

		// Target
		args.push_back(L"-T");
		args.push_back(L"ps_6_0");

#if defined(DASH_DEBUG)
		// Enable debug info
 		args.push_back(DXC_ARG_DEBUG);
		args.push_back(DXC_ARG_SKIP_OPTIMIZATIONS);
#else
		args.push_back(DXC_ARG_OPTIMIZATION_LEVEL3);
#endif

		args.push_back(DXC_ARG_WARNINGS_ARE_ERRORS);

		args.push_back(DXC_ARG_PACK_MATRIX_ROW_MAJOR);

		args.push_back(L"-Qstrip_debug");
		args.push_back(L"-Qstrip_reflect");

		for (const std::string& define : defines)
		{
			args.push_back(L"-D");
			args.push_back(UTF8ToWideString(define).c_str());
		}

		LOG_INFO << "begin compile : " << fileName;

		DxcBuffer buffer{};
		buffer.Encoding = DXC_CP_ACP;
		buffer.Ptr = source->GetBufferPointer();
		buffer.Size = source->GetBufferSize();

		ComPtr<IDxcResult> compiledResult;
		DX_CALL(mCompiler->Compile(&buffer, args.data(), static_cast<UINT32>(args.size()), mIncludeHandler.Get(), IID_PPV_ARGS(&compiledResult)));

		ComPtr<IDxcBlobUtf8> errors;
		DX_CALL(compiledResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&errors), nullptr));

		if (errors != nullptr && errors->GetStringLength() != 0)
		{
			LOG_INFO << fileName << " Compile error : \n" << errors->GetStringPointer();
		}
		else
		{
			LOG_INFO << fileName << " Compile succeed.";
		}

		HRESULT status;
		DX_CALL(compiledResult->GetStatus(&status));
		if (FAILED(status))
		{
			LOG_INFO << fileName << " Compile failed.";
			return nullptr;
		}

		ComPtr<IDxcBlob> Shader = nullptr;
		ComPtr<IDxcBlobUtf16> ShaderName = nullptr;
		DX_CALL(compiledResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&Shader), &ShaderName));

		return Shader;
	}

}