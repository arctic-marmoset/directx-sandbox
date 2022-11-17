// Light properties

static const float3 kLightPos = { 2.0f, 2.0f, 2.0f };
static const float3 kLightColor = { 1.0f, 1.0f, 1.0f };
static const float kLightIntensity = 1.0f;

static const float kAmbientLight = 0.3f;

static const float kAttnCoeff0 = 1.0f;
static const float kAttnCoeff2 = 0.0075f;

// Object properties

static const float3 kMaterialColor = { 0.6f, 0.2f, 0.2f };
static const float kSpecularIntensity = 0.5f;
static const float kSpecularPower = 10.0f;

float4 main(float3 worldPos : POSITION, float3 normal : NORMAL) : SV_Target
{
    // Get light distance and direction
    const float3 lVec = kLightPos - worldPos;
    const float lNorm = length(lVec);
    const float3 lCap = lVec / lNorm;
    
    // Calculate attenuation at this distance
    const float attn = 1 / (kAttnCoeff0 + kAttnCoeff2 * (lNorm * lNorm));
    
    // Calculate diffuse component
    const float diffuseLight = attn * kLightIntensity * max(0.0f, dot(lCap, normal));
    
    // Calculate specular component
    const float3 rCap = reflect(lCap, normal);
    const float rCosine = max(0.0f, dot(rCap, normalize(worldPos)));
    const float specularLight = attn * kSpecularIntensity * pow(rCosine, kSpecularPower);
    
    // Reflected colour should only be whatever exists in both the light and the material
    const float3 diffuseColor = kLightColor * kMaterialColor;
    
    // Observed object colour should be: 
    // diffuse component * diffuse colour +
    // ambient component * diffuse colour +
    // specular component * light colour
    const float3 observedColor = saturate((diffuseLight + kAmbientLight) * diffuseColor + specularLight * kLightColor);
    
    return float4(observedColor, 1.0f);
}
