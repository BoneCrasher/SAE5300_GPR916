// Lighting buffers to hold all light sources
cbuffer Lighting : register(b0) 
{
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
    float4 negL_normalized = -L_normalized;

    float4 N_normalized = normalize(input.normal);

    float angleOfIncidenceValue_cos = dot(N_normalized, negL_normalized);

    float lambertFactor = angleOfIncidenceValue_cos;


    // float4 fragmentColor = angleOfIncidenceValue_cos * input.color;
    float  ambientFactor = 0.2f;
    float4 ambientColor  = float4(237.0f, 145.0f, 92.0f, 255.0f) / 255.0f;
    
    float4 fragmentColor = lambertFactor * input.color;

    // Ongoing

    return clamp(((ambientFactor * ambientColor) + fragmentColor), 0.0f, 1.0f);

}