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
// Holds the current object's world transform
// and inverse transpose used for normal mapping
// normal corrections, due to non-uniform scaling.
cbuffer Object : register(b2)
{
    float4x4 world;
    float4x4 invTransposeWorld;
}

// Vertex Input struct as defined in the InputLayout.
struct VertexInput
{
    float4 position : POSITION0;
    float4 normal   : NORMAL0;
    float4 tangent  : TANGENT0;
    float4 uv       : TEXCOORD0;
    float4 color    : COLOR0;
};

struct VertexOutput
{
    float4 position : SV_POSITION;
};

VertexOutput main(VertexInput input) {
    float4 position  = float4(input.position.xyz, 1.0f);
    
    LightInfo light = lights[lightIndex];

    float4x4 lightViewProjection      = mul(light.projection, light.view[light.lightViewIndex]);
    float4x4 lightWorldViewProjection = mul(lightViewProjection, world);
        
    VertexOutput output;
    output.position    = mul(lightWorldViewProjection, position);
    return output;    
}