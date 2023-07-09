#include "PCH.h"
#include "ShaderTechnique.h"

namespace Dash
{
    FShaderPassRef& FShaderTechnique::AddShaderPass(const std::string& passName, const std::vector<FShaderCreationInfo>& creationInfos,
        const FBlendState& blendState, const FRasterizerState& rasterizerState, const FDepthStencilState& depthStencilState)
    {  
        FShaderPassRef& shaderPassRef = mShaderPasses.emplace_back(FShaderPass::MakeShaderPass(mName + "_" + passName, creationInfos, blendState, rasterizerState, depthStencilState)); 

        return shaderPassRef;
    }

    const FShaderPassRef& FShaderTechnique::GetShaderPass(uint32_t index) const
    {
        ASSERT(index < mShaderPasses.size());

        return mShaderPasses[index];
    }
}