Texture2D DisplayTexture : register(t0);
SamplerState StaticSampler : register(s3);

struct VSInput
{
	float3 Position : POSITION;
	//float3 Normal : NORMAL;
	//float4 Tangent : TANGENT;
	float2 UV : TEXCOORD;
	float4 Color : COLOR;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : COLOR;
};

/*
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
*/

PSInput VS_Main(VSInput input)
{
	PSInput output;

	output.Position = float4(input.Position, 1.0f);
	output.Color = input.Color;
	output.UV = input.UV;

	return output;
}

float4 PS_Main(PSInput input) : SV_Target0
{
	float4 Color = float4(input.UV.x, input.UV.y, 0.0f, 1.0f);
	return Color;
}

float4 PS_SampleColor(PSInput input) : SV_Target0
{
	return DisplayTexture.Sample(StaticSampler, input.UV);
}
