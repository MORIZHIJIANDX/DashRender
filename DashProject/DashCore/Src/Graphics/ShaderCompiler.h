#pragma once

#include <wtypes.h>
#include <Unknwn.h>
#include "ThirdParty/dxc/inc/d3d12shader.h"
#include "ThirdParty/dxc/inc/dxcapi.h"
#include <wrl.h>

namespace Dash
{
	class ShaderCompiler
	{
	public:
		void Init();
		void CompileShader();
	private:
		Microsoft::WRL::ComPtr<IDxcUtils> mUtils;
		Microsoft::WRL::ComPtr<IDxcCompiler3> mCompiler;
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> mIncludeHandler;
	};
}