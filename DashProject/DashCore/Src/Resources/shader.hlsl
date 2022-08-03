
struct VSInput
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float4 Tangent : TANGENT;
	float4 Color : COLOR;
	float2 UV : TEXCOORD;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD0;
	float4 Color : COLOR;
	float4 Normal : TEXCOORD1;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

cbuffer FrameBuffer : register(b0)
{
	matrix ViewMatrix;
	matrix ProjectionMatrix;
	matrix WorldMatrix;
	matrix InversetransposedWorldMatrix;
	float4 LightDirection;
	float4 LightColor;
	float TotalTime;
	float2 Speed;
};


PSInput VSMain(VSInput input)
{
	PSInput output;
	
	matrix viewProj = mul(ViewMatrix, ProjectionMatrix);

	output.Position = mul(float4(input.Position, 1.0f), viewProj);

	output.UV = input.UV;

	output.Color = input.Color;

	output.Normal = mul(float4(input.Normal, 0.0f), InversetransposedWorldMatrix);

	return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	//return g_texture.Sample(g_sampler, input.UV);

	//return float4( (sin(TotalTime) + 1.0f) /2.0f, (cos(TotalTime) + 1.0f) / 2.0f, 0.5f, 1.0f);

	//return float4(input.Color.xyz * 0.5f + 0.5f, 1.0f);

	float4 Color = max(dot(normalize(input.Normal.xyz), LightDirection.xyz), 0.0f) * LightColor * input.Color;
	return Color;
}
