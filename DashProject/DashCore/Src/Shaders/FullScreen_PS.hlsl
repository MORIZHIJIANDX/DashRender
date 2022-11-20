
struct VSInput
{
	float3 Position : POSITION;
	//float3 Normal : NORMAL;
	//float4 Tangent : TANGENT;
	//float4 Color : COLOR;
	float2 UV : TEXCOORD;
};

struct PSInput
{
	float4 Position : SV_POSITION;
	float2 UV : TEXCOORD0;
	//float4 Color : COLOR;
	//float4 Normal : TEXCOORD1;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

/*
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
*/

cbuffer FrameBuffer : register(b0)
{
	matrix ProjectionMatrix;
};


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
	return g_texture.Sample(g_sampler, uv);
}


/*
PSInput VS_Main(VSInput input)
{
	PSInput output;

	output.Position = float4(input.Position, 1.0f);

	output.UV = input.UV;

	return output;
}

float4 PS_Main(PSInput input) : SV_TARGET
{
	float4 Color = float4(input.UV.x, input.UV.y, 0.0f, 1.0f);
	//float4 Color = float4(0.9f, 0.5f, 0.1f, 1.0f);
	return Color;
}
*/
