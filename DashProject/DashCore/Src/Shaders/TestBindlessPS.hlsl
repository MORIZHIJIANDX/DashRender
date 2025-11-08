#include "BindlessCommon.hlsli"

BINDLESS_SRV(Texture2D, InputTexture);
BINDLESS_UAV(RWTexture2D<float4>, OutputTexture);

float CalcLuminance(float3 color)
{
    return dot(color, float3(0.299f, 0.587f, 0.114f));
}

[numthreads(16, 16, 1)]
void CS_Main(uint3 DTid : SV_DispatchThreadID)
{
    float4 Color = InputTexture[DTid.xy];
    float Luminance = CalcLuminance(Color.rgb);
    OutputTexture[DTid.xy] = float4(Luminance, Luminance, Luminance, Color.a);
}