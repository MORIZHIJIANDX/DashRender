#include "BindlessCommon.hlsli"

cbuffer MaterialConstantBuffer : register(b0)
{
    float4 Color;
};

BINDLESS_SRV(Texture2D, SceneTexture);
BINDLESS_SAMPLER(SamplerState, DepthTextureSampler);
BINDLESS_UAV(RWTexture2D<float>, OutputTexture);

float4 main() : SV_TARGET
{
    float4 SceneColor = SceneTexture.Sample(DepthTextureSampler, float2(0.0f, 0.0f));
    return SceneColor * Color;
}