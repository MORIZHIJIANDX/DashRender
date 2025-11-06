#include "StaticSamplerState.hlsli"

struct InstanceDataType
{
    float4 InstanceColor;
    float4x4 InstanceModelMatrix;
};

StructuredBuffer<InstanceDataType> InstanceData;

cbuffer FrameConstantBuffer
{
    float4x4 ViewProjectionMatrix;
};

cbuffer ObjectConstantBuffer
{
    float4x4 ModelMatrix;
};

cbuffer MaterialConstantBuffer
{
    float4 Color;
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    uint matrixId : COLOR_InstanceID;
    uint colorId : SV_InstanceID;
};
            
struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float4 instanceColor : COLOR0;
};
            
PS_INPUT VS_Main(VS_INPUT input)
{
    PS_INPUT output;
    //float4x4 MVPMatrix = mul(ModelMatrix, ViewProjectionMatrix);
    float4x4 MVPMatrix = mul(InstanceData[input.colorId].InstanceModelMatrix, ViewProjectionMatrix);
    output.pos = mul(float4(input.pos, 1.f), MVPMatrix);
    output.normal = input.normal;
    output.uv = input.uv;
    output.instanceColor = InstanceData[input.matrixId].InstanceColor;
    return output;
}

Texture2D BaseColorTexture;

float4 PS_Main(PS_INPUT input) : SV_Target
{
    //float4 out_col = float4(input.uv, 0.0f, 0.0f);
    //float4 out_col = float4(input.normal * Color.rgb, 0.0f);
    
    //float4 out_col = BaseColorTexture.Sample(Sampler_Static, input.uv) * Color;
    float4 out_col = BaseColorTexture.Sample(LinearClampStaticSampler, input.uv) * input.instanceColor * Color;
    return out_col;
}