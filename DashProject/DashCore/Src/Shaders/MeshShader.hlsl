cbuffer FrameConstantBuffer : register(b0)
{
    float4x4 ModelViewProjectionMatrix;
};

/*
cbuffer MaterialConstantBuffer : register(b1)
{
    float4 Color;
};
*/

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};
            
struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};
            
PS_INPUT VS_Main(VS_INPUT input)
{
    PS_INPUT output;
    output.pos = mul(float4(input.pos, 1.f), ModelViewProjectionMatrix);
    output.normal = input.normal;
    output.uv = input.uv;
    return output;
}

float4 PS_Main(PS_INPUT input) : SV_Target
{
    //float4 out_col = float4(input.uv, 0.0f, 0.0f);
    float4 out_col = float4(input.normal, 0.0f);
    return out_col;
}