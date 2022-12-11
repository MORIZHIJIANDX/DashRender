#include "PCH.h"
#include "ShaderMap.h"

namespace Dash
{
	using namespace Microsoft::WRL;

	void FShaderMap::Init()
	{
		for (size_t i = 0; i < MAX_PARALLEL_SHADER_COMPILER; i++)
		{
			mCompilers[i].Init();
		}
	}

	void FShaderMap::Destroy()
	{
		std::lock_guard<std::mutex> lock(mShaderMapMutex);
		mShaderResourceMap.clear();
	}

	FShaderResource FShaderMap::LoadShader(const FShaderCreationInfo& info)
	{
		std::lock_guard<std::mutex> lock(mShaderMapMutex);
		size_t shaderHash = info.GetShaderHash();
		if (!mShaderResourceMap.contains(shaderHash))
		{
			FShaderResourceRef shaderResource;
			FDX12CompiledShader compiledShader = mCompilers[0].CompileShader(info);
			if (compiledShader.IsValid())
			{	
				shaderResource.ShaderResource.Init(compiledShader.CompiledShaderBlob, compiledShader.ShaderReflector, info);

				mShaderResourceMap[shaderHash] = shaderResource;
			}
			else
			{
				ASSERT(false);
				return shaderResource.ShaderResource;
			}
		}

		mShaderResourceMap[shaderHash].AddRef();
		return mShaderResourceMap[shaderHash].ShaderResource;
	}

	void FShaderMap::ReleaseShader(const FShaderResource& info)
	{
		std::lock_guard<std::mutex> lock(mShaderMapMutex);
		size_t shaderHash = info.GetShaderHash();
		if (mShaderResourceMap.contains(shaderHash) && mShaderResourceMap[shaderHash].Release())
		{
			mShaderResourceMap.erase(shaderHash);
		}	
	}

}