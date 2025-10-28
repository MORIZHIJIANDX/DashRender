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

	FShaderResourceRef FShaderMap::LoadShader(const FShaderCreationInfo& info)
	{
		FShaderMap& globalShaderMap = GetInstance();
		std::lock_guard<std::mutex> lock(GetInstance().mShaderMapMutex);		
		size_t shaderHash = info.GetShaderHash();
		if (!globalShaderMap.mShaderResourceMap.contains(shaderHash))
		{
			FShaderResourceRef shaderResourceref = MakeRefCounted<FShaderResource>();
			FDX12CompiledShader compiledShader = globalShaderMap.mCompilers[0].CompileShader(info);
			if (compiledShader.IsValid())
			{	
				shaderResourceref->Init(compiledShader.CompiledShaderBlob, compiledShader.ShaderReflector, info);

				globalShaderMap.mShaderResourceMap[shaderHash] = shaderResourceref;
			}
			else
			{
				ASSERT_MSG(false, "Failed to compile or load shader.");
			}
		}

		return globalShaderMap.mShaderResourceMap[shaderHash];
	}

	void FShaderMap::ReleaseShader(const FShaderResourceRef& shaderRef)
	{
		FShaderMap& globalShaderMap = GetInstance();
		std::lock_guard<std::mutex> lock(globalShaderMap.mShaderMapMutex);
		size_t shaderHash = shaderRef->GetShaderHash();
		if (globalShaderMap.mShaderResourceMap.contains(shaderHash))
		{
			globalShaderMap.mShaderResourceMap.erase(shaderHash);
		}	
	}

}