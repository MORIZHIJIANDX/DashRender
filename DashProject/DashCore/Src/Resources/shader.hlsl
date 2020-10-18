//struct VSInput
//{
//	float3 Position : POSITION;
//	float2 UV : TEXCOORD;
//};

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
	float2 UV : TEXCOORD;
};

Texture2D g_texture : register(t0);

//Texture2DArray g_texture : register(t0);

SamplerState g_sampler : register(s0);

cbuffer FrameBuffer : register(b0)
{
	//row_major matrix ViewMatrix;
	//row_major matrix ProjectionMatrix;
	matrix ViewMatrix;
	matrix ProjectionMatrix;
	float TotalTime;
	float2 Speed;
};


PSInput VSMain(VSInput input)
{
	PSInput output;
	
	matrix viewProj = mul(ViewMatrix, ProjectionMatrix);

	//output.Position = mul(float4(input.Position, 1.0f), ProjectionMatrix);

	output.Position = mul(float4(input.Position, 1.0f), viewProj);

	//output.Position.xy += sin(TotalTime) * Speed;

	output.UV = input.UV;

	return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	//return input.Color;
	return g_texture.Sample(g_sampler, input.UV);

	//return g_texture.Sample(g_sampler, float3(input.UV, 1));

	//return g_texture.SampleLevel(g_sampler, input.UV, 5);
}