#pragma once
#include "ShaderCompiler.h"

namespace Dash
{
	#define MAX_PARALLEL_SHADER_COMPILER 2

	class FShaderMap
	{
	public:
		void Init();
		void Destroy();

		FShaderResource LoadShader(const FShaderCreationInfo& info);
		void ReleaseShader(const FShaderResource& info);

	private:
		struct FShaderResourceRef
		{
			FShaderResource ShaderResource;
			int32_t RefCount = 0;

			void AddRef() { ++RefCount; }
			bool Release() 
			{ 
				--RefCount;
				if (RefCount <= 0)
				{
					return true;
				}

				return false;
			}
		};

		std::mutex mShaderMapMutex;
		std::unordered_map<size_t, FShaderResourceRef> mShaderResourceMap;
		FShaderCompiler mCompilers[MAX_PARALLEL_SHADER_COMPILER];
	};
}