// This thing comes from previous stages.
struct FragmentInput // LINEARLY INTERPOLATED PER FRAGMENT DATA(!!!)
{
    float4 position : SV_Position;
    float4 normal : NORMAL0;
    float4 color : COLOR0;
};

float4 main(FragmentInput input) : SV_Target0
{ 
    float4 fragmentColor = input.color;

    // Ongoing

    return fragmentColor;

}