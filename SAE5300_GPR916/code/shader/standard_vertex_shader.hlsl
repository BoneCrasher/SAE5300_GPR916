// Camera constant buffer to hold the current 
// camera view and projection matrices and
// other parameters.
cbuffer Camera : register(b0)
{
    float4x4 view;
    float4x4 projection;
}

// Object buffer to hold the current object's 
// world matrix and other parameters.
cbuffer Object : register(b1)
{
    float4x4 world;
}

//  Vertex Input struct as defined in the InputLayout.
struct VertexInput
{
    float4 position : POSITION0;
    float4 normal : NORMAL0;
    float4 color : COLOR0;
};

// This thing goes into the subsequent stages.
struct VertexOutput
{
    float4 position : SV_Position;
    float4 worldSpacePosition : POSITION0;
    float4 normal : NORMAL0;
    float4 color : COLOR0;
};

// Do It!
VertexOutput main(VertexInput input)
{ 
    float4 position = float4(input.position.xyz, 1.0f);
    float4 normal   = float4(input.normal.xyz,   0.0f);
    
    float4x4 viewProjection      = mul(projection, view);
    float4x4 worldViewProjection = mul(viewProjection, world);

    VertexOutput output;
    output.position           = mul(worldViewProjection, input.position);
    output.worldSpacePosition = mul(world, input.position);
    output.normal             = mul(world, input.normal);
    output.color              = input.color;
    return output;
}