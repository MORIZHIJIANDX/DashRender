#pragma once

#include <wtypes.h>
#include <Unknwn.h>
#include "ThirdParty/dxc/inc/d3d12shader.h"
#include "ThirdParty/dxc/inc/dxcapi.h"
#include <wrl.h>
#include "ShaderResource.h"

namespace Dash
{
	class FShaderCompiler
	{
	public:
		void Init();

		FileUtility::ByteArray CompileShader(const FShaderCreationInfo& info);

	protected:
		Microsoft::WRL::ComPtr<IDxcBlob> CompileShaderInternal(const FShaderCreationInfo& info);
		bool SaveShaderBlob(const FShaderCreationInfo& info, Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob);
		FileUtility::ByteArray LoadShaderBlob(const FShaderCreationInfo& info);
		
	protected:
		Microsoft::WRL::ComPtr<IDxcUtils> mUtils;
		Microsoft::WRL::ComPtr<IDxcCompiler3> mCompiler;
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> mIncludeHandler;
	};
}