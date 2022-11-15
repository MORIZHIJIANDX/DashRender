#include "PCH.h"
#include "ShaderCompiler.h"
#include "Utility/StringUtility.h"
#include "Utility/FileUtility.h"
#include "Utility/Hash.h"
#include "DX12Helper.h"

#include<vector> 
#include<algorithm>

namespace Dash
{
	using namespace Microsoft::WRL;

	void FShaderCreationInfo::Finalize()
	{
		std::string hasedName = FileName + EntryPoint;

		size_t hash = HashState(FileName.c_str(), FileName.size());
		hash = HashState(EntryPoint.c_str(), EntryPoint.size(), hash);

		std::sort(Defines.begin(), Defines.end());
		for (const std::string& define : Defines)
		{
			hash = HashState(define.c_str(), define.size(), hash);
		}

		ShaderHash = hash;
		HashedFileName = hasedName + "_" + FStringUtility::ToString(hash);
	}

	void ShaderCompiler::Init()
	{
		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&mUtils));

		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&mCompiler));

		//create default include handler
		mUtils->CreateDefaultIncludeHandler(&mIncludeHandler);
	}

	ComPtr<IDxcBlob> ShaderCompiler::CompileShaderInternal(const FShaderCreationInfo& info)
	{
		if (!FileUtility::IsPathExistent(info.FileName))
		{
			LOG_WARNING << "Cannot found file : " << info.FileName;
			return nullptr;
		}

		std::wstring wFileName = FStringUtility::UTF8ToWideString(info.FileName);
		
		LOG_INFO << "Load File : " << info.FileName;

		ComPtr<IDxcBlobEncoding> source = nullptr;
		DX_CALL(mUtils->LoadFile(wFileName.c_str(), nullptr, &source));

		std::vector<LPCWSTR> args;

		// Optional shader source file name for error reporting
		args.push_back(wFileName.c_str());

		// Entry point
		std::wstring wEntryPoint = FStringUtility::UTF8ToWideString(info.EntryPoint);

		args.push_back(L"-E");
		args.push_back(wEntryPoint.c_str());

		// Target
		std::wstring target = GetShaderTargetFromEntryPoint(info.EntryPoint);
		args.push_back(L"-T");
		args.push_back(target.c_str());

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
		
		std::string combinedStr;
		for (const std::string& define : info.Defines)
		{
			args.push_back(L"-D");
			std::wstring wDefine = FStringUtility::UTF8ToWideString(define);
			args.push_back(wDefine.c_str());

			combinedStr += define + "_";
		}

		LOG_INFO << "begin compile : " << info.FileName << ", entry point : " << info.EntryPoint << ", defines : " << combinedStr;

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
			LOG_INFO << info.FileName << " Compile error : \n" << errors->GetStringPointer();
		}
		else
		{
			LOG_INFO << info.FileName << " Compile succeed.";
		}

		HRESULT status;
		DX_CALL(compiledResult->GetStatus(&status));
		if (FAILED(status))
		{
			LOG_INFO << info.FileName << " Compile failed.";
			return nullptr;
		}

		ComPtr<IDxcBlob> Shader = nullptr;
		ComPtr<IDxcBlobUtf16> ShaderName = nullptr;
		DX_CALL(compiledResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&Shader), &ShaderName));

		return Shader;
	}

	std::wstring ShaderCompiler::GetShaderTargetFromEntryPoint(const std::string& entryPoint)
	{
		std::vector<std::string> splitStrs = FStringUtility::Split(entryPoint, "_");

		ASSERT(splitStrs.size() == 2);

		static const std::string targetLevel{"_6_5"};

		return FStringUtility::UTF8ToWideString(FStringUtility::ToLower(splitStrs[0]) + targetLevel);
	}
}