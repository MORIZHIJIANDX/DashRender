#include "PCH.h"
#include "ShaderMap.h"

namespace Dash
{
	using namespace Microsoft::WRL;

	void FShaderMap::Init()
	{
		for (size_t i = 0; i < MAX_PARALLEL_SHADER_COMPILER; i++)
		{
			GetInstance().mCompilers[i].Init();
		}
	}

	void FShaderMap::Destroy()
	{
		std::lock_guard<std::mutex> lock(GetInstance().mShaderMapMutex);
		GetInstance().mShaderResourceMap.clear();
	}

	FShaderResource& FShaderMap::LoadShader(const FShaderCreationInfo& info)
	{
		FShaderMap& globalShaderMap = GetInstance();
		std::lock_guard<std::mutex> lock(GetInstance().mShaderMapMutex);		
		size_t shaderHash = info.GetShaderHash();
		if (!globalShaderMap.mShaderResourceMap.contains(shaderHash))
		{
			FShaderResourceRef shaderResource;
			FDX12CompiledShader compiledShader = globalShaderMap.mCompilers[0].CompileShader(info);
			if (compiledShader.IsValid())
			{	
				shaderResource.ShaderResource.Init(compiledShader.CompiledShaderBlob, compiledShader.ShaderReflector, info);

				globalShaderMap.mShaderResourceMap[shaderHash] = shaderResource;
			}
			else
			{
				ASSERT_MSG(false, "Failed to compile or load shader.");
			}
		}

		globalShaderMap.mShaderResourceMap[shaderHash].AddRef();
		return globalShaderMap.mShaderResourceMap[shaderHash].ShaderResource;
	}

	void FShaderMap::ReleaseShader(const FShaderResource& info)
	{
		FShaderMap& globalShaderMap = GetInstance();
		std::lock_guard<std::mutex> lock(globalShaderMap.mShaderMapMutex);
		size_t shaderHash = info.GetShaderHash();
		if (globalShaderMap.mShaderResourceMap.contains(shaderHash) && globalShaderMap.mShaderResourceMap[shaderHash].Release())
		{
			globalShaderMap.mShaderResourceMap.erase(shaderHash);
		}	
	}

}