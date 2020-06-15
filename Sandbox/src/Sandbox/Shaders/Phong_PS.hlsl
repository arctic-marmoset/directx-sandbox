// Light properties

static const float3 kLightPos = { 0.0f, 0.0f, -3.0f };
static const float3 kLightColor = { 1.0f, 1.0f, 1.0f };
static const float kLightIntensity = 0.5f;

static const float kAmbientLight = 0.2f;

static const float kAttnCoeff = 0.0075f;

// Object properties

static const float3 kMaterialColor = { 0.6f, 0.2f, 0.2f };

float4 main(float3 worldPos : POSITION, float3 normal : NORMAL) : SV_Target
{
    // Get light distance and direction
    const float3 lVec = kLightPos - worldPos;
    const float lNorm = length(lVec);
    const float3 lCap = lVec / lNorm;
    
    // Calculate attenuation at this distance
    const float attn = 1 / (kAttnCoeff * (lNorm * lNorm));
    
    // Calculate diffuse component
    const float diffuseLight = attn * kLightIntensity * max(0.0f, dot(lCap, normal));
    
    // Reflected colour should only be whatever exists in both the light and the material
    const float3 diffuseColor = kLightColor * kMaterialColor;
    
    // Observed object colour should be diffuse component * diffuse color
    // plus ambient component * diffuse color
    const float3 observedColor = saturate((diffuseLight + kAmbientLight) * diffuseColor);
    
    return float4(observedColor, 1.0f);
}
