#pragma once

#include "ShaderPass.h"

namespace Dash
{
	class FShaderTechnique
	{
	public:
		FShaderTechnique(const std::string& name)
			: mName(name)
		{}

		FShaderPassRef& AddShaderPass(const std::string& passName, const std::vector<FShaderCreationInfo>& creationInfos, 
			const FBlendState& blendState, const FRasterizerState& rasterizerState, const FDepthStencilState& depthStencilState);

		const FShaderPassRef& GetShaderPass(uint32_t index) const;
		const std::vector<FShaderPassRef>& GetPasses() const { return mShaderPasses; }

	private: 
		std::string mName;
		std::vector<FShaderPassRef> mShaderPasses;
	};
}