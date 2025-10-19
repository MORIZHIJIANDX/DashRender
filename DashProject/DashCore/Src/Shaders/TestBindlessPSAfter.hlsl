#include "BindlessCommon.hlsli"

cbuffer MaterialConstantBuffer : register(b0)
{
    float4 Color;
};

// ===== Auto-generated Bindless CBuffer =====
// Generated from BINDLESS_* macro declarations
// Found 3 resources across 2 files
// Detected 1 existing cbuffers:
//   space0: MaterialConstantBuffer(b0, space0)
// Assigned register: b1, space0
// Last generated: 2025-10-19 14:35:46.9676963
//
// Processed files:
//   - TestBindlessPS.hlsl
//   - BindlessCommon.hlsli

cbuffer BindlessCBuffer : register(b1, space0)
{
    // SRV Resources
    uint BindlessSRV_SceneTexture; // Texture2D from H:\MyProject\DashRender\DashProject\DashCore\Src\Shaders\TestBindlessPS.hlsl
    // UAV Resources
    uint BindlessUAV_OutputTexture; // RWTexture2D<float> from H:\MyProject\DashRender\DashProject\DashCore\Src\Shaders\TestBindlessPS.hlsl
    // Sampler Resources
    uint BindlessSAMPLER_DepthTextureSampler; // SamplerState from H:\MyProject\DashRender\DashProject\DashCore\Src\Shaders\TestBindlessPS.hlsl
};

// ===== End of Auto-generated Section =====

BINDLESS_SRV(Texture2D, SceneTexture);
BINDLESS_SAMPLER(SamplerState, DepthTextureSampler);
BINDLESS_UAV(RWTexture2D<float>, OutputTexture);

float4 main() : SV_TARGET
{
    float4 SceneColor = SceneTexture.Sample(DepthTextureSampler, float2(0.0f, 0.0f));
    return SceneColor * Color;
}
