#pragma once
#include "ShaderResource.h"

namespace Dash
{
	class FShaderPass
	{
	public:
		void SetShader(EShaderStage stage, const FShaderCreationInfo& creationInfo);
		void Finalize();

	private:
		std::map<EShaderStage, FShaderResource*> mShaders;
		std::vector<FShaderParameter> mPassParameters;
	};
}