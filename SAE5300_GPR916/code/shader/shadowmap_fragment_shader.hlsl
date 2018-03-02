struct FragmentInput
{
    float4 position    : SV_POSITION;
};

float2 main(FragmentInput input) : SV_Target {
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}