#include "PCH.h"
#include "ShaderCompiler.h"
#include "Utility/StringUtility.h"
#include "Utility/Hash.h"
#include "DX12Helper.h"
#include "Utility/FileUtility.h"
#include "ShaderPreprocesser.h"

namespace Dash
{
	using namespace Microsoft::WRL;

	void FShaderCompiler::Init()
	{
		DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(mUtils.GetInitReference()));

		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(mCompiler.GetInitReference()));

		//create default include handler
		mUtils->CreateDefaultIncludeHandler(mIncludeHandler.GetInitReference());
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
		if (!FFileUtility::IsPathExistent(info.FileName))
		{
			DASH_LOG(LogTemp, Warning, "Cannot found file : {}", info.FileName);
			return FDX12CompiledShader{};
		}

		std::wstring wFileName = FStringUtility::UTF8ToWideString(info.FileName);
		
		DASH_LOG(LogTemp, Info, "Load File : {}", info.FileName);

		FShaderPreprocessdResult preprocessResult = FShaderPreprocesser::Process(info.FileName);

		bool dumpShader = true;
		if (dumpShader)
		{
			std::string dumpShaderDir = FFileUtility::CombinePath(FFileUtility::GetBasePath(info.FileName), "DumpShaders");
			std::string dumpShaderPath = FFileUtility::CombinePath(dumpShaderDir, FFileUtility::GetFileName(info.FileName));

			FFileUtility::WriteTextFileSync(dumpShaderPath, preprocessResult.ShaderCode);
		}

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

		args.push_back(L"-HV 2021");

		args.push_back(L"-enable-16bit-types");

		args.push_back(L"-Wno-parentheses-equality");
		
		std::string combinedStr;
		for (const std::string& define : info.Defines)
		{
			args.push_back(L"-D");
			std::wstring wDefine = FStringUtility::UTF8ToWideString(define);
			args.push_back(wDefine.c_str());

			combinedStr += define + "_";
		}

		DASH_LOG(LogTemp, Info, "Begin compile : {}, entry point : {}, defines : {}", info.FileName, info.EntryPoint, combinedStr);

		DxcBuffer buffer{};
		buffer.Encoding = DXC_CP_UTF8;
		buffer.Ptr = preprocessResult.ShaderCode.data();
		buffer.Size = preprocessResult.ShaderCode.size();

		TRefCountPtr<IDxcResult> compiledResult;
		DX_CALL(mCompiler->Compile(&buffer, args.data(), static_cast<UINT32>(args.size()), mIncludeHandler.GetReference(), IID_PPV_ARGS(compiledResult.GetInitReference())));

		TRefCountPtr<IDxcBlobUtf8> errors;
		TRefCountPtr<IDxcBlobWide> errorOutputName;
		DX_CALL(compiledResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errors.GetInitReference()), errorOutputName.GetInitReference()));

		if (errors != nullptr && errors->GetStringLength() != 0)
		{
			DASH_LOG(LogTemp, Error, "{}  Compile error : \n {}", info.FileName, errors->GetStringPointer());
		}
		else
		{
			DASH_LOG(LogTemp, Info, "{} Compile succeed.", info.FileName);
		}

		HRESULT status;
		DX_CALL(compiledResult->GetStatus(&status));
		if (FAILED(status))
		{
			DASH_LOG(LogTemp, Error, "{}  Compile failed.", info.FileName);
			return FDX12CompiledShader{};
		}

		TRefCountPtr<IDxcBlob> shaderBlob = nullptr;
		TRefCountPtr<IDxcBlobUtf16> shaderName = nullptr;
		DX_CALL(compiledResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(shaderBlob.GetInitReference()), shaderName.GetInitReference()));

		TRefCountPtr<ID3D12ShaderReflection> shaderReflector;

		TRefCountPtr<IDxcBlob> reflectionBlob;
		TRefCountPtr<IDxcBlobWide> reflectionOutputName;
		DX_CALL(compiledResult->GetOutput(DXC_OUT_REFLECTION, IID_PPV_ARGS(reflectionBlob.GetInitReference()), reflectionOutputName.GetInitReference()));
		if (reflectionBlob != nullptr)
		{
			// Create reflection interface.
			DxcBuffer reflectionData;
			reflectionData.Encoding = DXC_CP_ACP;
			reflectionData.Ptr = reflectionBlob->GetBufferPointer();
			reflectionData.Size = reflectionBlob->GetBufferSize();

			mUtils->CreateReflection(&reflectionData, IID_PPV_ARGS(shaderReflector.GetInitReference()));
		}

#if defined(DASH_DEBUG)
		TRefCountPtr<IDxcBlob> pdbBlob;
		TRefCountPtr<IDxcBlobWide> pdbOutputName;
		DX_CALL(compiledResult->GetOutput(DXC_OUT_PDB, IID_PPV_ARGS(pdbBlob.GetInitReference()), pdbOutputName.GetInitReference()));
		std::string pdbPath = info.GetHashedFileName() + PDB_BLOB_FILE_EXTENSION;

		if (FFileUtility::WriteBinaryFileSync(pdbPath, reinterpret_cast<unsigned char*>(pdbBlob->GetBufferPointer()), pdbBlob->GetBufferSize()))
		{
			DASH_LOG(LogTemp, Info, "Success to save compiled shader pdb : {}", pdbPath);
		}
		else
		{
			DASH_LOG(LogTemp, Error, "Failed to save compiled shader pdb : {}.", pdbPath);
		}
#endif

		FDX12CompiledShader compiledShader;
		compiledShader.CompiledShaderBlob = shaderBlob;
		compiledShader.ShaderRelectionBlob = reflectionBlob;
		compiledShader.ShaderReflector = shaderReflector;
		compiledShader.BindlessResourceMap = preprocessResult.BindlessResourceMap;

		return compiledShader;
	}

	bool FShaderCompiler::SaveShaderBlob(const FShaderCreationInfo& info, const FDX12CompiledShader& compiledShader)
	{
		std::string hasedShaderName = info.GetHashedFileName() + SHADER_BLOB_FILE_EXTENSION;

		if (FFileUtility::WriteBinaryFileSync(hasedShaderName, reinterpret_cast<unsigned char*>(compiledShader.CompiledShaderBlob->GetBufferPointer()), compiledShader.CompiledShaderBlob->GetBufferSize()))
		{
			DASH_LOG(LogTemp, Info, "Success to save compiled shader blob : {}", hasedShaderName);
		}
		else
		{
			DASH_LOG(LogTemp, Error, "Failed to save compiled shader blob : {}.", hasedShaderName);
		}

		std::string hasedRefectionName = info.GetHashedFileName() + REFLECTION_BLOB_FILE_EXTENSION;

		if (FFileUtility::WriteBinaryFileSync(hasedRefectionName, reinterpret_cast<unsigned char*>(compiledShader.ShaderRelectionBlob->GetBufferPointer()), compiledShader.ShaderRelectionBlob->GetBufferSize()))
		{
			DASH_LOG(LogTemp, Info, "Success to save shader reflection blob : {}", hasedRefectionName);
		}
		else
		{
			DASH_LOG(LogTemp, Error, "Failed to save shader reflection blob : {}.", hasedRefectionName);
		}

		SavePreprocessInfo(info, compiledShader);

		return true;
	}

	FDX12CompiledShader FShaderCompiler::LoadShaderBlob(const FShaderCreationInfo& info)
	{
		std::string hasedShaderName = info.GetHashedFileName() + SHADER_BLOB_FILE_EXTENSION;
		TRefCountPtr<IDxcBlobEncoding> compiledShaderBlob = LoadBlobFromFile(hasedShaderName);

		std::string reflectionFileName = info.GetHashedFileName() + REFLECTION_BLOB_FILE_EXTENSION;
		TRefCountPtr<IDxcBlobEncoding> reflectionBlob = LoadBlobFromFile(reflectionFileName);

		TRefCountPtr<ID3D12ShaderReflection> shaderReflector;

		// Create reflection interface.
		DxcBuffer reflectionData;
		reflectionData.Encoding = DXC_CP_ACP;
		reflectionData.Ptr = reflectionBlob->GetBufferPointer();
		reflectionData.Size = reflectionBlob->GetBufferSize();

		mUtils->CreateReflection(&reflectionData, IID_PPV_ARGS(shaderReflector.GetInitReference()));

		FDX12CompiledShader compiledShader;
		compiledShader.CompiledShaderBlob = compiledShaderBlob;
		compiledShader.ShaderRelectionBlob = reflectionBlob;
		compiledShader.ShaderReflector = shaderReflector;

		LoadPreprocessInfo(info, compiledShader);

		return compiledShader;
	}

	TRefCountPtr<IDxcBlobEncoding> FShaderCompiler::LoadBlobFromFile(const std::string& fileName)
	{
		TRefCountPtr<IDxcBlobEncoding> blob = nullptr;

		std::wstring wShaderFileName = FStringUtility::UTF8ToWideString(fileName);

		if (FFileUtility::IsPathExistent(fileName))
		{
			DX_CALL(mUtils->LoadFile(wShaderFileName.c_str(), nullptr, blob.GetInitReference()));

			DASH_LOG(LogTemp, Info, "Success to load shader blob : {}", fileName);
		}

		return blob;
	}

	bool FShaderCompiler::SavePreprocessInfo(const FShaderCreationInfo& info, const FDX12CompiledShader& compiledShader)
	{
		std::string preprocessdFileName = info.GetHashedFileName() + SHADER_PREPROCESS_FILE_EXTENSION;

		std::ostringstream builder;
		for (const auto& [key, value] : compiledShader.BindlessResourceMap) {
			builder << key << '=' << value << '\n';
		}

		return FFileUtility::WriteTextFileSync(preprocessdFileName, builder.str());
	}

	bool FShaderCompiler::LoadPreprocessInfo(const FShaderCreationInfo& info, FDX12CompiledShader& compiledShader)
	{
		std::string preprocessdFileName = info.GetHashedFileName() + SHADER_PREPROCESS_FILE_EXTENSION;

		std::optional<std::string> fileText = FFileUtility::ReadTextFileSync(preprocessdFileName);
		if (!fileText) {
			return false;
		}

		compiledShader.BindlessResourceMap.clear();

		std::string line;
		std::istringstream input(*fileText);
		while (std::getline(input, line)) {
			// 跳过空行
			if (line.empty()) continue;

			// 查找 '=' 分隔符
			size_t pos = line.find('=');
			if (pos != std::string::npos) {
				std::string key = line.substr(0, pos);
				std::string value = line.substr(pos + 1);
				compiledShader.BindlessResourceMap[key] = value;
			}
		}

		return true;
	}
}