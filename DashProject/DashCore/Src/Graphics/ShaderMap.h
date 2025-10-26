#pragma once

#include "GraphicTypesFwd.h"
#include "ShaderCompiler.h"

namespace Dash
{
	#define MAX_PARALLEL_SHADER_COMPILER 2

	class FShaderMap
	{
	public:
		static void Init();
		static void Destroy();

		static FShaderResourceRef LoadShader(const FShaderCreationInfo& info);
		static void ReleaseShader(const FShaderResourceRef& shaderRef);

	private:
		static FShaderMap& GetInstance()  
		{
			static FShaderMap globalInstance; 
			return globalInstance;
		}

		std::mutex mShaderMapMutex;
		std::unordered_map<size_t, FShaderResourceRef> mShaderResourceMap;
		FShaderCompiler mCompilers[MAX_PARALLEL_SHADER_COMPILER];
	};


}