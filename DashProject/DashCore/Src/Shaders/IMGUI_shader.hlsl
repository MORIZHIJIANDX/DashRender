cbuffer constantBuffer : register(b0)
{
    float4x4 ProjectionMatrix; 
};

struct VS_INPUT
{
    float2 pos : POSITION;
    float2 uv  : TEXCOORD0;
    half4 col : COLOR_Short;
};
            
struct PS_INPUT
{
    float4 pos : SV_POSITION;
    half4 col : COLOR;
    float2 uv  : TEXCOORD0;
};
            
PS_INPUT VS_Main(VS_INPUT input)
{
    PS_INPUT output;
    //output.pos = mul( ProjectionMatrix, float4(input.pos.xy, 0.f, 1.f));
    output.pos = mul(float4(input.pos.xy, 0.f, 1.f), ProjectionMatrix);
    output.col = input.col;
    output.uv  = input.uv;
    return output;
}
 
//SamplerState sampler0 : register(s0); 
SamplerState StaticSampler : register(s3);
Texture2D texture0 : register(t0); 

float4 PS_Main(PS_INPUT input) : SV_Target
{
    float4 out_col = input.col * texture0.Sample(StaticSampler, input.uv);
    //float4 out_col = float4(input.col.r, input.col.g, input.col.b, input.col.a);
    return out_col; 
}