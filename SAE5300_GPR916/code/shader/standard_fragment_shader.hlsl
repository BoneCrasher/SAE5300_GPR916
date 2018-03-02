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

struct LightInfo { 
    float4x4 view[6];
    float4x4 projection;
    float4 position;
    float4 direction;
    float4 color;
    uint   type;
    uint   lightViewIndex;
    uint   unused1;
    uint   unused2;
    float  intensity;
    float  distance;
    float  hotSpotAngle;
    float  falloffAngle;
};

cbuffer Lighting : register(b1) 
{
    LightInfo lights[4];
    int       lightIndex;    
    int       unused0;
    int       unused1;
    int       unused2;
}

struct FragmentInput
{    
    float4 position          : SV_Position;
    float4 position_ws       : POSITION0;
    float4 position_ls[4][6] : POSITION1;
    float3 tangent           : TANGENT0;
    float3 normal            : NORMAL0;
    float3 binormal          : NORMAL1;
    float4 uv                : TEXCOORD0;
    float4 color             : COLOR0;
};

static const float PI = 3.14159265f;

Texture2D   diffuseTexture  : register(t0);
Texture2D   specularTexture : register(t1);
Texture2D   glossTexture    : register(t2);
Texture2D   normalTexture   : register(t3);
TextureCubeArray shadowMaps      : register(t4);
SamplerState           samplerState : register(s0);
SamplerComparisonState shadowMapSamplerState : register(s1);

float3 sampleCube(
    const float3 v)
{
    float faceIndex = -1;

	float3 vAbs = abs(v);
	float  ma;
	float2 uv;
	if(vAbs.z >= vAbs.x && vAbs.z >= vAbs.y)
	{
		faceIndex = v.z < 0.0 ? 5.0 : 4.0;
		ma = 0.5 / vAbs.z;
		uv = float2(v.z < 0.0 ? -v.x : v.x, -v.y);
	}
	else if(vAbs.y >= vAbs.x)
	{
		faceIndex = v.y < 0.0 ? 3.0 : 2.0;
		ma = 0.5 / vAbs.y;
		uv = float2(v.x, v.y < 0.0 ? -v.z : v.z);
	}
	else
	{
		faceIndex = v.x < 0.0 ? 1.0 : 0.0;
		ma = 0.5 / vAbs.x;
		uv = float2(v.x < 0.0 ? v.z : -v.z, -v.y);
	}

	return float3((uv * ma + 0.5).xy, faceIndex);
}

struct phongLightingResult {
    float f_lambert;
    float beta;
    float cone;
    float f_specular;
    float4 color;
};

phongLightingResult phongLighting(
        float3 N,
        float3 L,
        float  d_LS,
        float  I_L,
        float3 R,
        float3 V,
        float4 C_D,
        float4 C_L,
        float  n,
        float  s_specular)
{
    phongLightingResult result;

    // Lambert
    float f_lambert  = dot(N, L); // alpha
    float f_specular = 0.0f;
    float beta       = 0.0f;
    float cone       = 0.0f;
    if(f_lambert > 0.0f) {
        beta       = max(0.0f, dot(R, V));
        cone       = pow(beta, n);
        f_specular = s_specular * cone;
    }
    
    // Falloff
    float3 falloff_proportions = { 0.5f, 0.3f, 0.01f};
    float  falloff             = I_L * (1.0f / (falloff_proportions.x * pow(d_LS, 2) + falloff_proportions.y * d_LS + falloff_proportions.z));
    
    float  s_diffuse = 1.0f;

    result.f_lambert  = f_lambert;
    result.f_specular = f_specular;
    result.beta       = beta;
    result.cone       = cone;
    result.color      = falloff * ((C_L * C_D * s_diffuse * f_lambert) + (C_L * f_specular));
    
    return result;
}

struct shadowResult {
    float shadowFactor; 
};

shadowResult applyShadows(
        phongLightingResult phong, 
        float4 P_ls[6],
        float3 L,
        float  d_L,
        float  I_L,
        float  d_Shadow) 
{ 
    // Remove shadow acne, also on sloped surfaces!
    float bias = 0.005 * tan(acos(phong.f_lambert)); 
    bias = clamp(bias, 0.0f, 0.01);
        
    float3 cubeSampleCoordinates = sampleCube(-L); // Determine face and uv inside it to properly sample surface light space position.  
    float  surface_d             = (P_ls[cubeSampleCoordinates.z].z / P_ls[cubeSampleCoordinates.z].w);
   
    surface_d *= d_L;
    surface_d -= bias;
    
    float shadowFactor         = 1.0f;
    float athmosphereDiffusion = 0.2f;

    float light_d = (d_Shadow * d_L);    
    if(surface_d > light_d)
      shadowFactor = 0.0f;    
    
    shadowResult result;
    result.shadowFactor = shadowFactor;
    return result;
}

float4 main(FragmentInput input) : SV_Target {        

    // Read colors from textures for TextureMapping
    float4 t_diffuseColor  = diffuseTexture.Sample(samplerState, input.uv.xy);
    float4 t_specularColor = specularTexture.Sample(samplerState, input.uv.xy);
    float4 t_glossColor    = glossTexture.Sample(samplerState, input.uv.xy);
    float4 t_normalColor   = normalTexture.Sample(samplerState, input.uv.xy);   

    float3x3 TBN 
    = {
        normalize(input.tangent), 
        normalize(input.binormal),
        normalize(input.normal)
    };
    TBN = transpose(TBN);
    
    float3 N_unpacked_normalized_tangentspace = normalize(((2.0f * t_normalColor) - 1.0f).xyz);
    float3 N_unpacked_normalized_worldspace   = normalize(mul(TBN, N_unpacked_normalized_tangentspace));    
    float3 N_normalized = N_unpacked_normalized_worldspace;

    //return float4(((N_normalized + 1.0f) * 0.5f), 1.0f);
    
    float3 V = (cameraPosition - input.position_ws).xyz;
    float3 V_normalized = normalize(V);

    float  gloss_bias        = 7;
    float  gloss_base        = 64 - gloss_bias;
    float  glossiness_factor = gloss_bias + (gloss_base * t_glossColor.r);
    
    float  s_specular = t_specularColor.r;

    phongLightingResult lighting[4];
    shadowResult        shadow[4];

    uint affectingLightCount = 0;

    for(uint k=0; k<4; ++k) {        
        LightInfo light = lights[k];
    
        int    lightType      = light.type;
        int    lightIntensity = light.intensity;
        int    lightDistance  = light.distance;
        float4 lightPosition  = float4(light.position.xyz, 1.0f);
        float4 lightDirection = float4(light.direction.xyz, 0.0f);

        if(lightIntensity == 0)
            continue;
            
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
        
        float3 R = -reflect(L_normalized, N_normalized);
        float3 R_normalized = normalize(R);

        float4 t_shadow = shadowMaps.Sample(samplerState, float4(-L_normalized, k));

        lighting[affectingLightCount]
            = phongLighting(
                N_normalized, 
                L_normalized,
                length(L),
                lightIntensity,
                R_normalized, 
                V_normalized,
                t_diffuseColor,
                light.color,
                glossiness_factor,
                s_specular);
        shadow[affectingLightCount]
            = applyShadows(
                lighting[affectingLightCount],
                input.position_ls[k],
                L_normalized,
                lightDistance,
                lightIntensity,
                t_shadow.r);

        ++affectingLightCount;
    }
    
    float4  f_light  = 0.0f;
    for(k=0; k<affectingLightCount; ++k) {
        f_light  += lighting[k].color * shadow[k].shadowFactor;
    }
   
    return saturate( f_light );
}