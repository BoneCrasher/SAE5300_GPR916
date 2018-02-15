// Lighting buffers to hold all light sources
cbuffer Lighting : register(b0) 
{
    float4 cameraPosition;
    float4 lightPosition;
}

// This thing comes from previous stages.
struct FragmentInput // LINEARLY INTERPOLATED PER FRAGMENT DATA(!!!)
{
    float4 position : SV_Position;
    float4 worldSpacePosition : POSITION0;
    float4 normal : NORMAL0;
    float4 color : COLOR0;
};

static const float PI = 3.14159265f;

float4 main(FragmentInput input) : SV_Target0
{ 
    float4 L = (lightPosition - input.worldSpacePosition);
    float4 L_normalized  = normalize(L);

    float4 N_normalized = normalize(input.normal);

    float4 R = -reflect(L_normalized, N_normalized);
    float4 R_normalized = normalize(R);

    float4 V = (cameraPosition - input.worldSpacePosition);
    float4 V_normalized = normalize(V);

    float angleOfIncidenceValue_cos = max(0.0f, dot(N_normalized.xyz, L_normalized.xyz));

    float lambertFactor = angleOfIncidenceValue_cos;

    float  glossiness_factor = 96.0f;
    float  specularFactor    = pow(max(0.0f, dot(R_normalized.xyz, V_normalized.xyz)), glossiness_factor);
    float4 specularColor     = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    // float4 fragmentColor = angleOfIncidenceValue_cos * input.color;
    float  ambientFactor = 0.0f;
    float4 ambientColor  = float4(237.0f, 145.0f, 92.0f, 255.0f) / 255.0f;
   
    float4 ambient  = ambientFactor  * ambientColor;
    float4 lambert  = lambertFactor  * float4(0.7f, 0.7f, 0.7f, 1.0f); 
    float4 specular = specularFactor * specularColor;
    return clamp(ambient + lambert + specular, 0.0f, 1.0f);

}