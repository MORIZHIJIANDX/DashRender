#include "PCH.h"
#include "ShaderCompiler.h"
#include "Utility/StringUtility.h"
#include "Utility/Hash.h"
#include "DX12Helper.h"
#include "Utility/FileUtility.h"

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

	FDX12CompiledShader FShaderCompiler::CompileShader(const FShaderCreationInfo& info)
	{
		FDX12CompiledShader compiledShader;

		if (info.IsOutOfDate())
		{
			compiledShader = CompileShaderInternal(info);

			if (compiledShader.IsValid())
			{
				SaveShaderBlob(info, compiledShader);
			}
		}
		else
		{
			compiledShader = LoadShaderBlob(info);
		}

		return compiledShader;
	}

	FDX12CompiledShader FShaderCompiler::CompileShaderInternal(const FShaderCreationInfo& info)
	{
		if (!FileUtility::IsPathExistent(info.FileName))
		{
			LOG_WARNING << "Cannot found file : " << info.FileName;
			return FDX12CompiledShader{};
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
			return FDX12CompiledShader{};
		}

		ComPtr<IDxcBlob> shaderBlob = nullptr;
		ComPtr<IDxcBlobUtf16> shaderName = nullptr;
		DX_CALL(compiledResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), &shaderName));

		ComPtr<ID3D12ShaderReflection> shaderReflector;

		ComPtr<IDxcBlob> reflectionBlob;
		compiledResult->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(&reflectionBlob), nullptr);
		if (reflectionBlob != nullptr)
		{
			// Create reflection interface.
			DxcBuffer reflectionData;
			reflectionData.Encoding = DXC_CP_ACP;
			reflectionData.Ptr = reflectionBlob->GetBufferPointer();
			reflectionData.Size = reflectionBlob->GetBufferSize();

			mUtils->CreateReflection(&reflectionData, IID_PPV_ARGS(&shaderReflector));
		}

		FDX12CompiledShader compiledShader;
		compiledShader.CompiledShaderBlob = shaderBlob;
		compiledShader.ShaderRelectionBlob = reflectionBlob;
		compiledShader.ShaderReflector = shaderReflector;

		return compiledShader;
	}

	bool FShaderCompiler::SaveShaderBlob(const FShaderCreationInfo& info, const FDX12CompiledShader& compiledShader)
	{
		std::string hasedShaderName = info.GetHashedFileName() + SHADER_BLOB_FILE_EXTENSION;

		if (FileUtility::WriteBinaryFileSync(hasedShaderName, reinterpret_cast<unsigned char*>(compiledShader.CompiledShaderBlob->GetBufferPointer()), compiledShader.CompiledShaderBlob->GetBufferSize()))
		{
			LOG_INFO << "Success to save compiled shader blob : " << hasedShaderName;
		}
		else
		{
			LOG_ERROR << "Failed to save compiled shader blob : " << hasedShaderName;
		}

		std::string hasedRefectionName = info.GetHashedFileName() + REFLECTION_BLOB_FILE_EXTENSION;

		if (FileUtility::WriteBinaryFileSync(hasedRefectionName, reinterpret_cast<unsigned char*>(compiledShader.ShaderRelectionBlob->GetBufferPointer()), compiledShader.ShaderRelectionBlob->GetBufferSize()))
		{
			LOG_INFO << "Success to save shader reflection blob : " << hasedRefectionName;
		}
		else
		{
			LOG_ERROR << "Failed to save shader reflection blob : " << hasedRefectionName;
		}

		return false;
	}

	FDX12CompiledShader FShaderCompiler::LoadShaderBlob(const FShaderCreationInfo& info)
	{
		std::string hasedShaderName = info.GetHashedFileName() + SHADER_BLOB_FILE_EXTENSION;
		ComPtr<IDxcBlobEncoding> compiledShaderBlob = LoadBlobFromFile(hasedShaderName);

		std::string reflectionFileName = info.GetHashedFileName() + REFLECTION_BLOB_FILE_EXTENSION;
		ComPtr<IDxcBlobEncoding> reflectionBlob = LoadBlobFromFile(reflectionFileName);

		ComPtr<ID3D12ShaderReflection> shaderReflector;

		// Create reflection interface.
		DxcBuffer reflectionData;
		reflectionData.Encoding = DXC_CP_ACP;
		reflectionData.Ptr = reflectionBlob->GetBufferPointer();
		reflectionData.Size = reflectionBlob->GetBufferSize();

		mUtils->CreateReflection(&reflectionData, IID_PPV_ARGS(&shaderReflector));

		FDX12CompiledShader compiledShader;
		compiledShader.CompiledShaderBlob = compiledShaderBlob;
		compiledShader.ShaderRelectionBlob = reflectionBlob;
		compiledShader.ShaderReflector = shaderReflector;

		return compiledShader;
	}

	ComPtr<IDxcBlobEncoding> FShaderCompiler::LoadBlobFromFile(const std::string& fileName)
	{
		ComPtr<IDxcBlobEncoding> blob = nullptr;

		std::wstring wShaderFileName = FStringUtility::UTF8ToWideString(fileName);

		if (FileUtility::IsPathExistent(fileName))
		{
			DX_CALL(mUtils->LoadFile(wShaderFileName.c_str(), nullptr, &blob));
		}

		return blob;
	}

}