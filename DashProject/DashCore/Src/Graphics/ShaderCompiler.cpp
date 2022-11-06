#include "PCH.h"
#include <wtypes.h>
#include <Unknwn.h>
#include "ThirdParty/dxc/inc/d3d12shader.h"
#include "ThirdParty/dxc/inc/dxcapi.h"
#include <wrl.h>
#include "ShaderCompiler.h"

namespace Dash
{
	using namespace Microsoft::WRL;

	void ShaderCompiler::CompileShader()
	{
		ComPtr<IDxcCompiler3> compiler;
		DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler)); 
	}
}