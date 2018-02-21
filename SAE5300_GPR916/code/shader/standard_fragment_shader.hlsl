// Lighting buffers to hold all light sources
cbuffer Lighting : register(b0) 
{
    float4x4 lights[4];
    float4   cameraPosition;
    float4   cameraDirection;
    int      displayMode;
    int      lightIndex;
    int      unused1;
    int      unused2;
}

// This thing comes from previous stages.
struct FragmentInput // LINEARLY INTERPOLATED PER FRAGMENT DATA(!!!)
{
    float4 position           : SV_Position;
    float4 worldSpacePosition : POSITION0;
    float4 normal             : NORMAL0;
    float4 tangent            : TANGENT0;
    float4 uv                 : TEXCOORD0;
    float4 color              : COLOR0;
};

static const float PI = 3.14159265f;

Texture2D    textures[4];
SamplerState samplerState;

float4 main(FragmentInput input) : SV_Target0
{ 
    float4 t_diffuseColor  = textures[0].Sample(samplerState, input.uv.xy);
    float4 t_specularColor = textures[1].Sample(samplerState, input.uv.xy);
    float4 t_glossColor    = textures[2].Sample(samplerState, input.uv.xy);

    switch(displayMode) { 
        case 5: // NormalMap
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
        case 4: // GlossMap
        return t_glossColor.rrrr;
        case 3: // SpecularMap
        return t_specularColor.rrrr;
        case 2: // DiffuseMap
        return t_diffuseColor;
        case 1:
        break;
    }

    float4x4 light = lights[lightIndex];

    float4 L = float4(0.0f, 0.0f, 0.0f, 0.0f); 
    
    int    lightType      = light._41;
    int    lightIntensity = light._42;
    float4 lightPosition  = float4(light._14_24_34, 1.0f);
    float4 lightDirection = float4(light._31_32_33, 0.0f);

    switch(lightType) {
        case 0: // Directional
        L = -lightDirection;
        break;
        case 1: // Point
        L = (lightPosition - input.worldSpacePosition);
        break;
        case 2: // Spot
        L = (lightPosition - input.worldSpacePosition);
        break;
    }    

    float4 L_normalized  = normalize(L);

    float4 N_normalized = normalize(input.normal);

    float4 R = -reflect(L_normalized, N_normalized);
    float4 R_normalized = normalize(R);

    float4 V = -cameraDirection; // (cameraPosition - input.worldSpacePosition);
    float4 V_normalized = normalize(V);

    float angleOfIncidenceValue_cos = dot(N_normalized.xyz, L_normalized.xyz);

    float lambertFactor = max(0.0f, angleOfIncidenceValue_cos);
    
    float  gloss_bias        = 7;
    float  gloss_base        = 64 - gloss_bias;
    float  glossiness_factor = gloss_bias + (gloss_base * t_glossColor.r);
    float  specularFactor    = 0.0f;
    if(angleOfIncidenceValue_cos >= 0) {
        float dotProduct = dot(R_normalized.xyz, V_normalized.xyz);
        float powProduct = pow(dotProduct, glossiness_factor);
        specularFactor = t_specularColor.r * powProduct;
    }

    float4 specularColor = float4(1.0f, 1.0f, 1.0f, 1.0f); 
    
    float diffuseFactor = 1.0f - specularFactor;

    // float4 fragmentColor = angleOfIncidenceValue_cos * input.color;
    float  ambientFactor = 0.1f;
    float4 ambientColor  = float4(255.0, 255.0f, 255.0, 255.0f) / 255.0f;
    float4 ambient  = ambientFactor  * ambientColor;
    float4 lambert  = lambertFactor  * diffuseFactor * t_diffuseColor; 
    float4 specular = specularFactor * specularColor;

    return clamp(ambient + lightIntensity * (lambert + specular), 0.0f, 1.0f);

}