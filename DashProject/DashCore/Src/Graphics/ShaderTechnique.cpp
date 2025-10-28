#include "PCH.h"
#include "ShaderTechnique.h"

namespace Dash
{
    FShaderTechniqueRef FShaderTechnique::MakeShaderTechnique(const std::string& techniqueName)
    {
        FShaderTechniqueRef newTechnique = MakeRefCounted<FShaderTechnique>();
        newTechnique->mName = techniqueName;

        return newTechnique;
    }

    FShaderPassRef& FShaderTechnique::AddShaderPass(const std::string& passName, const std::vector<FShaderCreationInfo>& creationInfos,
        const FBlendState& blendState, const FRasterizerState& rasterizerState, const FDepthStencilState& depthStencilState)
    {  
        FShaderPassRef& shaderPassRef = mShaderPasses.emplace_back(FShaderPass::MakeGraphicShaderPass(mName + "_" + passName, creationInfos, blendState, rasterizerState, depthStencilState)); 

        return shaderPassRef;
    }

    uint32 FShaderTechnique::GetNumPasses() const
    {
        return static_cast<uint32>(mShaderPasses.size());
    }

    const FShaderPassRef& FShaderTechnique::GetShaderPass(uint32 index) const
    {
        ASSERT(index < mShaderPasses.size());

        return mShaderPasses[index];
    }
}