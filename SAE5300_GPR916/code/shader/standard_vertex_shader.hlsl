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

// Holds the current object's world transform
// and inverse transpose used for normal mapping
// normal corrections, due to non-uniform scaling.
cbuffer Object : register(b1)
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
    float4   position           : SV_Position;
    float4   worldSpacePosition : POSITION0;
    float3x3 TBN                : TANGENT0;
    float4   uv                 : TEXCOORD0;
    float4   color              : COLOR0;
};

VertexOutput main(VertexInput input) {
    float4 position  = float4(input.position.xyz, 1.0f);
    float4 normal    = normalize(float4(input.normal.xyz,   0.0f));
    float4 tangent   = normalize(float4(input.tangent.xyz,  0.0f));
    float4 bitangent = normalize(float4(cross(normal.xyz, tangent.xyz), 0.0f));
    
    float4x4 viewProjection      = mul(projection, view);
    float4x4 worldViewProjection = mul(viewProjection, world);

    VertexOutput output;
    output.uv    = input.uv;
    output.color = input.color;

    output.position           = mul(worldViewProjection, input.position);
    output.worldSpacePosition = mul(world, input.position);

    float3x3 TBN 
    = {
        tangent.xyz,
        bitangent.xyz,
        normal.xyz
    };
    float3x3 normalMatrix 
    = {
        invTransposeWorld[0].xyz,
        invTransposeWorld[1].xyz,
        invTransposeWorld[2].xyz
    };

    output.TBN = mul(TBN, normalMatrix);
    
    return output;    
}