#include "BindlessCommon.hlsli"

cbuffer MaterialConstantBuffer
{
    float4 Color;
};

// ===== Auto-generated Bindless CBuffer =====
// Generated from BINDLESS_* macro declarations
// Found 3 resources across 2 files
// Detected 1 existing cbuffers:
// MaterialConstantBuffer
//
// Processed files:
//   - TestBindlessPS.hlsl
//   - BindlessCommon.hlsli

cbuffer BindlessCBuffer
{
    // SRV Resources
    uint BindlessSRV_SceneTexture; // Texture2D<float4> from Src\Shaders\TestBindlessPS.hlsl
    // UAV Resources
    uint BindlessUAV_OutputTexture; // RWTexture2D<float> from Src\Shaders\TestBindlessPS.hlsl
    // Sampler Resources
    uint BindlessSAMPLER_DepthTextureSampler; // SamplerState from Src\Shaders\TestBindlessPS.hlsl
};

// ===== End of Auto-generated Section =====

BINDLESS_SRV(Texture2D<float4>, SceneTexture);
BINDLESS_SAMPLER(SamplerState, DepthTextureSampler);
BINDLESS_UAV(RWTexture2D<float>, OutputTexture);

float4 main() : SV_TARGET
{
    float4 SceneColor = SceneTexture.Sample(DepthTextureSampler, float2(0.0f, 0.0f));
    return SceneColor * Color;
}
