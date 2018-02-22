// Holds the current camera view and projection
// matrices, as well as its position and direction
// (if directional or spotlight).
cbuffer Camera : register(b0)
{
    float4x4 view;
    float4x4 projection;
    float4   cameraPosition;
    float4   cameraDirection;
}

cbuffer Lighting : register(b1) 
{
    float4x4 lights[4];
    int      lightIndex;    
}

struct FragmentInput
{
    float4 position    : SV_Position;
    float4 position_ws : POSITION0;
    float3 tangent     : TANGENT0;
    float3 normal      : NORMAL0;
    float3 binormal    : NORMAL1;
    float4 uv          : TEXCOORD0;
    float4 color       : COLOR0;
};

static const float PI = 3.14159265f;

Texture2D    textures[4];
SamplerState samplerState;

float4 phongLighting(
        float3 N,
        float3 L,
        float  d_L,
        float  I_L,
        float3 R,
        float3 V,
        float4 C_D,
        float  n,
        float  s_specular)
{
    // Lambert
    float f_lambert  = dot(N, L); // alpha
    float f_specular = 0.0f;
    if(f_lambert > 0.0f) {
        float beta = max(0.0f, dot(R, V));
        float cone = pow(beta, n);
        f_specular = s_specular * cone;
    }

    float  falloff   = I_L * (1.0f / pow(d_L, 2));

    float4 C_L       = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float  s_diffuse = 1.0f;

    float4 result = falloff * ((C_D * s_diffuse * f_lambert) + (C_L * f_specular));
    result = clamp(result, 0.0f, 1.0f);
    
    return result;
}

float4 main(FragmentInput input) : SV_Target0 {
    // Read colors from textures for TextureMapping
    float4 t_diffuseColor  = textures[0].Sample(samplerState, input.uv.xy);
    float4 t_specularColor = textures[1].Sample(samplerState, input.uv.xy);
    float4 t_glossColor    = textures[2].Sample(samplerState, input.uv.xy);
    float4 t_normalColor   = textures[3].Sample(samplerState, input.uv.xy);

    float4x4 light = lights[lightIndex];
    
    int    lightType      = light._41;
    int    lightIntensity = light._42;
    float4 lightPosition  = float4(light._14_24_34, 1.0f);
    float4 lightDirection = float4(light._31_32_33, 0.0f);
    
    float4 L = float4(0.0f, -1.0f, 0.0f, 0.0f);
    switch(lightType) {
        case 0: // Directional
        L = -lightDirection;
        break;
        case 1: // Point
        L = (lightPosition - input.position_ws);
        break;
        case 2: // Spot
        L = (lightPosition - input.position_ws);
        break;
    }    
    float3 L_normalized = normalize(L.xyz);

    float3x3 TBN 
    = {
        normalize(input.tangent), 
        -normalize(input.binormal),
        normalize(input.normal)
    };
    TBN = transpose(TBN);
    
    float3 N_unpacked_normalized_tangentspace = normalize(((2.0f * t_normalColor) - 1.0f).xyz);
    float3 N_unpacked_normalized_worldspace   = normalize(mul(TBN, N_unpacked_normalized_tangentspace));    
    float3 N_normalized = N_unpacked_normalized_worldspace;

    //return float4(((N_normalized + 1.0f) * 0.5f), 1.0f);

    float3 R = -reflect(L_normalized, N_normalized);
    float3 R_normalized = normalize(R);
    
    float3 V = (cameraPosition - input.position_ws).xyz;
    float3 V_normalized = normalize(V);

    float  gloss_bias        = 7;
    float  gloss_base        = 64 - gloss_bias;
    float  glossiness_factor = gloss_bias + (gloss_base * t_glossColor.r);
    
    float  s_specular = t_specularColor.r;

    return phongLighting(
                N_normalized, 
                L_normalized,
                length(L),
                lightIntensity,
                R_normalized, 
                V_normalized,
                t_diffuseColor,
                glossiness_factor,
                s_specular);
}