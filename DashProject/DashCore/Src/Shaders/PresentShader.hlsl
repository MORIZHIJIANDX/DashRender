#include "StaticSamplerState.hlsli"

Texture2D DisplayTexture;

void VS_Main(
	in uint VertID : SV_VertexID,
	out float4 Pos : SV_Position,
	out float2 Tex : TexCoord0)
{
	// Texture coordinates range [0, 2], but only [0, 1] appears on screen.
    Tex = float2(uint2(VertID, VertID << 1) & 2);
    Pos = float4(lerp(float2(-1, 1), float2(1, -1), Tex), 0, 1);
}

float4 PS_Main(float4 position : SV_Position, float2 uv : TexCoord0) : SV_Target0
{
    return DisplayTexture.Sample(LinearClampStaticSampler, uv);
}
 