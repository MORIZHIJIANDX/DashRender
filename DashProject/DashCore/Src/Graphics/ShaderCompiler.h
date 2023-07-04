#pragma once

#include <wtypes.h>
#include <Unknwn.h>
#include "dxc/inc/d3d12shader.h"
#include "dxc/inc/dxcapi.h"
#include <wrl.h>
#include "ShaderResource.h"

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
		Microsoft::WRL::ComPtr<IDxcBlobEncoding> LoadBlobFromFile(const std::string& fileName);
		
	protected:
		Microsoft::WRL::ComPtr<IDxcUtils> mUtils;
		Microsoft::WRL::ComPtr<IDxcCompiler3> mCompiler;
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> mIncludeHandler;
	};
}