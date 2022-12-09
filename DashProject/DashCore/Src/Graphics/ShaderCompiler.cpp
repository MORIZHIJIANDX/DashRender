#include "PCH.h"
#include "ShaderCompiler.h"
#include "Utility/StringUtility.h"
#include "Utility/Hash.h"
#include "DX12Helper.h"

#include<vector> 
#include<algorithm>

namespace Dash
{
	using namespace Microsoft::WRL;

	void FShaderCompiler::Init()
	{
		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&mUtils));

		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&mCompiler));

		//create default include handler
		mUtils->CreateDefaultIncludeHandler(&mIncludeHandler);
	}

	FileUtility::ByteArray FShaderCompiler::CompileShader(const FShaderCreationInfo& info)
	{
		FileUtility::ByteArray compiledShader = FileUtility::NullFile;

		if (info.IsOutOfDate())
		{
			ComPtr<IDxcBlob> shaderBlob = CompileShaderInternal(info);

			if (shaderBlob)
			{
				SaveShaderBlob(info, shaderBlob);

				compiledShader = std::make_shared<std::vector<unsigned char>>(shaderBlob->GetBufferSize());
				std::memcpy(compiledShader->data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());
			}
		}
		else
		{
			compiledShader = LoadShaderBlob(info);
		}

		return compiledShader;
	}

	ComPtr<IDxcBlob> FShaderCompiler::CompileShaderInternal(const FShaderCreationInfo& info)
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
		std::wstring target = FStringUtility::UTF8ToWideString(info.GetShaderTarget());
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

	bool FShaderCompiler::SaveShaderBlob(const FShaderCreationInfo& info, Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob)
	{
		if (FileUtility::WriteBinaryFileSync(info.GetHashedFileName(), reinterpret_cast<unsigned char*>(shaderBlob->GetBufferPointer()), shaderBlob->GetBufferSize()))
		{
			LOG_INFO << "Success to save compiled shader : " << info.GetHashedFileName();
		}
		else
		{
			LOG_ERROR << "Failed to save compiled shader : " << info.GetHashedFileName();
		}

		return false;
	}

	FileUtility::ByteArray FShaderCompiler::LoadShaderBlob(const FShaderCreationInfo& info)
	{
		FileUtility::ByteArray compiledShaderFile = FileUtility::ReadBinaryFileSync(info.GetHashedFileName());

		std::wstring wFileName = FStringUtility::UTF8ToWideString(info.GetHashedFileName());
		ComPtr<IDxcBlobEncoding> source = nullptr;
		DX_CALL(mUtils->LoadFile(wFileName.c_str(), nullptr, &source));

		std::shared_ptr<std::vector<unsigned char>> compiledShader = std::make_shared<std::vector<unsigned char>>(source->GetBufferSize());
		std::memcpy(compiledShader->data(), source->GetBufferPointer(), source->GetBufferSize());

		if (compiledShaderFile != FileUtility::NullFile)
		{
			LOG_INFO << "Success to load compiled shader : " << info.GetHashedFileName();
		}
		else
		{
			LOG_ERROR << "Failed to load compiled shader : " << info.GetHashedFileName();
		}

		return compiledShaderFile;
	}
}