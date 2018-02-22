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

cbuffer Other : register(b2) 
{
    int      displayMode;
    int      unused0;
    int      unused1;
    int      unused2;
}

// This thing comes from previous stages.
struct FragmentInput // LINEARLY INTERPOLATED PER FRAGMENT DATA(!!!)
{
    float4 position           : SV_Position;
    float4 worldSpacePosition : POSITION0;
    float4 lightDirection     : NORMAL0;
    float4 viewDirection      : NORMAL1;
    float3x3 TBN              : TANGENT0;
    float4 uv                 : TEXCOORD0;
    float4 color              : COLOR0;
};

static const float PI = 3.14159265f;

Texture2D    textures[4];
SamplerState samplerState;


float3 TangentSpaceToTargetSpaceNormal(float3 input, float3 normal, float3 bitangent, float3 tangent, bool doTranspose)
{
    /*
    |Tx Ty Tz| |Vx| |TxVx + TyVy + TzVz|
    |Bx By Bz|*|Vy|=|BxVx + ByVy + BzVz|
    |Nx Ny Nz| |Vz| |NxVx + NyVy + NzVz|
    
    |Tx Bx Nx| |Vx| |TxVx + BxVy + NxVz|
    |Ty By Ny|*|Vy|=|TyVx + ByVy + NyVz|
    |Tz Bz Nz| |Vz| |TzVx + BzVy + NzVz|
    */
    //if (transpose)
    //{
    //    return float3(
    //    (tangent.x*input.x + bitangent.x*input.y + normal.x*input.z), 
    //    (tangent.y*input.x + bitangent.y*input.y + normal.y*input.z), 
    //    (tangent.z*input.x + bitangent.z*input.y + normal.z*input.z)
    //    );
    //}
    //else
    //{
    //    return float3(
    //    (tangent.x   * input.x + tangent.y   * input.y + tangent.z   * input.z),
    //    (bitangent.x * input.x + bitangent.y * input.y + bitangent.z * input.z),
    //    (normal.x    * input.x + normal.y    * input.y + normal.z    * input.z)
    //    );
    //}

    float3x3 TBN = float3x3(normalize(tangent), normalize(bitangent), normalize(normal));
    if (doTranspose) 
        TBN = transpose(TBN);

    float3 n = normalize(mul(TBN, input));

    return n;
}

float4 main(FragmentInput input) : SV_Target0
{ 
    float4 t_diffuseColor  = textures[0].Sample(samplerState, input.uv.xy);
    float4 t_specularColor = textures[1].Sample(samplerState, input.uv.xy);
    float4 t_glossColor    = textures[2].Sample(samplerState, input.uv.xy);
    float4 t_normalColor   = textures[3].Sample(samplerState, input.uv.xy);

    switch(displayMode) { 
        case 5: // NormalMap
        return t_normalColor;
        case 4: // GlossMap
        return t_glossColor.rrrr;
        case 3: // SpecularMap
        return t_specularColor.rrrr;
        case 2: // DiffuseMap
        return t_diffuseColor;
        case 1:
        break;
    }
    

    float4 L = float4(0.0f, -1.0f, 0.0f, 0.0f); 

    float4x4 light = lights[lightIndex];
    
    int    lightType      = light._41;
    int    lightIntensity = light._42;

    float4 lightPosition  = float4(light._14_24_34, 1.0f);
    float4 lightDirection = float4(light._31_32_33, 0.0f);

    lightPosition.x = -lightPosition.x;

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

    float3x3 TBN = float3x3(normalize(input.TBN[0]), normalize(input.TBN[1]), normalize(input.TBN[2]));
    // Unpack 0.0..1.0 range color to -1.0 .. 1.0 normal deviation range
    float3 N = mul(transpose(TBN), normalize((2 * t_normalColor) - 1).rgb);

    // Tangent space to world space
    // float3 N = float3(0.0f, 1.0f, 0.0f);// OriginalNormal; 

    float4 N_normalized = normalize(float4(N, 0.0f));

    float4 R = -reflect(L_normalized, N_normalized);
    float4 R_normalized = normalize(R);

    float4 V = -cameraDirection;
    float4 V_normalized = normalize(V);

    float angleOfIncidenceValue_cos = dot(N_normalized.xyz, L_normalized.xyz);

    float lambertFactor = max(0.0f, angleOfIncidenceValue_cos);
    
    float  gloss_bias        = 7;
    float  gloss_base        = 64 - gloss_bias;
    float  glossiness_factor = gloss_bias + (gloss_base * t_glossColor.r);
    float  specularFactor    = 0.0f;
    if(angleOfIncidenceValue_cos >= 0) {
        float dotProduct = max(0.0f, dot(R_normalized.xyz, V_normalized.xyz));
        float powProduct = pow(dotProduct, glossiness_factor);
        specularFactor = t_specularColor.r * powProduct;
    }

    float4 specularColor = float4(1.0f, 1.0f, 1.0f, 1.0f); 
    
    float diffuseFactor = 1.0f; // 1.0f - specularFactor;

    // float4 fragmentColor = angleOfIncidenceValue_cos * input.color;
    float  ambientFactor = 0.1f;
    float4 ambientColor  = float4(255.0f, 255.0f, 255.0f, 255.0f) / 255.0f;
    float4 ambient  = ambientFactor  * ambientColor;
    float4 lambert  = lambertFactor  * diffuseFactor * t_diffuseColor; 
    float4 specular = specularFactor * specularColor;
    
    return clamp(ambient + lightIntensity * (lambert /*+ specular*/), 0.0f, 1.0f);

}