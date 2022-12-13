
struct VSInput
{
	float3 Position : POSITION;
	float2 UV : TEXCOORD;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD0;
};

Texture2D DisplayTexture : register(t0);
SamplerState StaticSampler : register(s3);


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
	float4 Color = float4(uv.x, uv.y, 0.0f, 1.0f);
	return Color;
}

float4 PS_SampleColor(float4 position : SV_Position, float2 uv : TexCoord0) : SV_Target0
{
	return DisplayTexture.Sample(StaticSampler, uv);
}
