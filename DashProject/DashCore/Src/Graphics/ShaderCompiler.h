#pragma once

#include <wtypes.h>
#include <Unknwn.h>
#include "ThirdParty/dxc/inc/d3d12shader.h"
#include "ThirdParty/dxc/inc/dxcapi.h"
#include <wrl.h>
#include "Utility/FileUtility.h"

namespace Dash
{
	struct FShaderCreationInfo
	{
	public:
		explicit FShaderCreationInfo(const std::string& fileName, const std::string& entryPoint, const std::vector<std::string>& defines = {})
			: FileName (fileName)
			, EntryPoint (entryPoint)
			, Defines (defines)
		{}

		void Finalize();
		size_t GetShaderHash() const { return ShaderHash; }
		std::string GetHashedFileName() const { return HashedFileName; }
		bool IsOutOfDate() const;

		std::string FileName;
		std::string EntryPoint;
		std::vector<std::string> Defines;

	protected:
		std::string HashedFileName;
		size_t ShaderHash = 0;
	};

	class ShaderCompiler
	{
	public:
		void Init();

		FileUtility::ByteArray CompileShader(const FShaderCreationInfo& info);

	protected:
		Microsoft::WRL::ComPtr<IDxcBlob> CompileShaderInternal(const FShaderCreationInfo& info);
		bool SaveShaderBlob(const FShaderCreationInfo& info, Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob);
		FileUtility::ByteArray LoadShaderBlob(const FShaderCreationInfo& info);
		std::wstring GetShaderTargetFromEntryPoint(const std::string& entryPoint);

	protected:
		Microsoft::WRL::ComPtr<IDxcUtils> mUtils;
		Microsoft::WRL::ComPtr<IDxcCompiler3> mCompiler;
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> mIncludeHandler;
	};
}