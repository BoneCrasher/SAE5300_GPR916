// Camera constant buffer to hold the current 
// camera view and projection matrices and
// other parameters.
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
    int      unused0;  
    int      unused1;  
    int      unused2;    
}

// Object buffer to hold the current object's 
// world matrix and other parameters.
cbuffer Object : register(b2)
{
    float4x4 world;
    float4x4 invTransposeWorld;
}

//  Vertex Input struct as defined in the InputLayout.
struct VertexInput
{
    float4 position : POSITION0;
    float4 normal   : NORMAL0;
    float4 tangent  : TANGENT0;
    float4 uv       : TEXCOORD0;
    float4 color    : COLOR0;
};

// This thing goes into the subsequent stages.
struct VertexOutput
{
    float4 position           : SV_Position;
    float4 worldSpacePosition : POSITION0;
    float4 lightDirection     : NORMAL0;
    float4 viewDirection      : NORMAL1;
    float3x3 TBN              : TANGENT0;
    float4 uv                 : TEXCOORD0;
    float4 color              : COLOR0;
};

// Do It!
VertexOutput main(VertexInput input)
{ 
    float4 position = float4(input.position.xyz, 1.0f);
    float4 normal   = float4(input.normal.xyz,   0.0f);
    float4 tangent  = float4(input.tangent.xyz,  0.0f);
    
    float4x4 viewProjection      = mul(projection, view);
    float4x4 worldViewProjection = mul(viewProjection, world);

    VertexOutput output;
    output.position           = mul(worldViewProjection, input.position);
    output.worldSpacePosition = mul(world, input.position);
    output.uv                 = input.uv;
    output.color              = input.color;

    float4 n = float4(normal);
    float4 t = float4(tangent);
    float4 b = float4(normalize(cross(n.xyz, t.xyz)), 0.0f);
    
    float4x4 TBN = float4x4(t, b, n, float4(0.0f, 0.0f, 0.0f, 1.0f));
    TBN = mul(TBN, invTransposeWorld);  
 
    float4x4 light = lights[lightIndex];

    output.TBN = float3x3(TBN[0].xyz, TBN[1].xyz, TBN[2].xyz);
   
    return output;
}