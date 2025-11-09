#pragma once

#include <wtypes.h>
#include <Unknwn.h>
#include "dxc/inc/dxcapi.h"
#include "ShaderResource.h"
#include "Utility/RefCounting.h"

namespace Dash
{
	class FShaderCompiler
	{
	public:
		void Init();

		FDX12CompiledShader CompileShader(const FShaderCreationInfo& info);

	protected:
		FDX12CompiledShader CompileShaderInternal(const FShaderCreationInfo& info);
		bool SaveShaderBlob(const FShaderCreationInfo& info, const FDX12CompiledShader& compiledShader);
		FDX12CompiledShader LoadShaderBlob(const FShaderCreationInfo& info);
		TRefCountPtr<IDxcBlobEncoding> LoadBlobFromFile(const std::string& fileName);

		bool SavePreprocessInfo(const FShaderCreationInfo& info, const FDX12CompiledShader& compiledShader);
		bool LoadPreprocessInfo(const FShaderCreationInfo& info, FDX12CompiledShader& compiledShader);
		
	protected:
		TRefCountPtr<IDxcUtils> mUtils;
		TRefCountPtr<IDxcCompiler3> mCompiler;
		TRefCountPtr<IDxcIncludeHandler> mIncludeHandler;
	};
}