Texture2D ColorBuffer : register(t0);
SamplerState Sampler_Static : register(s3);

float LinearToSRGB(float value)
{
    // Approximately pow(color, 1.0 / 2.2)
    return value < 0.0031308 ? 12.92 * value : 1.055 * pow(abs(value), 1.0 / 2.4) - 0.055;
}

float3 LinearToSRGB(float3 color)
{
    // Approximately pow(color, 1.0 / 2.2)
    return float3(LinearToSRGB(color.r), LinearToSRGB(color.g), LinearToSRGB(color.b));
}

float CalcLuminance(float3 color)
{
    return dot(color, float3(0.299f, 0.587f, 0.114f));
}

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
    float4 Color = ColorBuffer.Sample(Sampler_Static, uv);
    Color.rgb = CalcLuminance(Color.rgb);
    Color.rgb = LinearToSRGB(Color.rgb);

    return Color;
}
 