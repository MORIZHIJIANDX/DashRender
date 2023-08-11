#pragma once

#include "ShaderPass.h"

namespace Dash
{
	class FShaderTechnique;
	using FShaderTechniqueRef = std::shared_ptr<FShaderTechnique>;

	class FShaderTechnique
	{
	public:

		static FShaderTechniqueRef MakeShaderTechnique(const std::string& techniqueName);

		FShaderPassRef& AddShaderPass(const std::string& passName, const std::vector<FShaderCreationInfo>& creationInfos, 
			const FBlendState& blendState, const FRasterizerState& rasterizerState, const FDepthStencilState& depthStencilState);

		int32_t GetNumPasses() const;
		const FShaderPassRef& GetShaderPass(uint32_t index) const;
		const std::vector<FShaderPassRef>& GetPasses() const { return mShaderPasses; }

	private: 
		std::string mName;
		std::vector<FShaderPassRef> mShaderPasses;
	};
}