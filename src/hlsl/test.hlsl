#define GRootSignature \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX), " \
    "DescriptorTable(SRV(t0), visibility = SHADER_VISIBILITY_PIXEL), " \
    "StaticSampler(s0, filter = FILTER_MIN_MAG_MIP_LINEAR, visibility = SHADER_VISIBILITY_PIXEL)"

cbuffer _ : register(b0)
{
    float4x4 GScreenToClipSpace;
}

Texture2D GTexture : register(t0);
SamplerState GSampler : register(s0);

[RootSignature(GRootSignature)]
void main_vs(
    in float2 InPosition : _Position,
    in float2 InTexcoord : _Texcoords,
    in float4 InColor : _Color,
    out float4 OutPosition : SV_Position,
    out float2 OutTexcoord : _Texcoords,
    out float4 OutColor : _Color)
{
    OutPosition = mul(float4(InPosition, 0.0f, 1.0f), GScreenToClipSpace);
    OutTexcoord = InTexcoord;
    OutColor = InColor;
}

[RootSignature(GRootSignature)]
void main_ps(
    in float4 InPosition : SV_Position,
    in float2 InTexcoord : _Texcoords,
    in float4 InColor : _Color,
    out float4 OutColor : SV_Target0)
{
    OutColor = InColor * GTexture.Sample(GSampler, InTexcoord);
}
