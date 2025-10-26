#pragma once

#include "GraphicTypesFwd.h"
#include "ShaderPass.h"

namespace Dash
{
	class FShaderTechnique : public FRefCount
	{
	public:

		static FShaderTechniqueRef MakeShaderTechnique(const std::string& techniqueName);

		FShaderPassRef& AddShaderPass(const std::string& passName, const std::vector<FShaderCreationInfo>& creationInfos, 
			const FBlendState& blendState, const FRasterizerState& rasterizerState, const FDepthStencilState& depthStencilState);

		uint32 GetNumPasses() const;
		const FShaderPassRef& GetShaderPass(uint32 index) const;
		const std::vector<FShaderPassRef>& GetPasses() const { return mShaderPasses; }

	private: 
		std::string mName;
		std::vector<FShaderPassRef> mShaderPasses;
	};
}