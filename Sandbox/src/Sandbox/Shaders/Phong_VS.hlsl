struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float3 worldPos : POSITION;
    float3 normal : NORMAL;
    float4 pos : SV_Position;
};

cbuffer ConstBuffer
{
    matrix kModel;
    matrix kModelViewProj;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    const float4 objPos = float4(input.pos, 1.0f);
    
    // Get screen position from (object position * ModelViewProj matrix)
    output.pos = mul(objPos, kModelViewProj);
    
    // Get world position from (object position * Model matrix)
    output.worldPos = (float3)mul(objPos, kModel);
    
    // Get world normal from (object normal * Model matrix)
    output.normal = mul(input.normal, (float3x3)kModel);
    
    return output;
}
