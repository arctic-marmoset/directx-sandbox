struct VS_INPUT
{
    float2 Pos : POSITION;
    float3 Color : COLOR;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR;
};

cbuffer ConstBuffer
{
    matrix Transform;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.Pos = mul(float4(input.Pos, 0.0f, 1.0f), Transform);
    output.Color = float4(input.Color, 1.0f);
    return output;
}
