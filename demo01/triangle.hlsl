#define root_sig \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX)"

struct ps_data {
    float4 position : SV_Position;
};

#if defined VS_TRIANGLE

struct vs_data {
    float3 position : POSITION;
};

struct cs_data {
    float4x4 world2proj;
};
ConstantBuffer<cs_data> s_cb : register(b0);

[RootSignature(root_sig)]
ps_data vs_triangle(vs_data input)
{
    ps_data output;
    output.position = mul(float4(input.position, 1.0f), s_cb.world2proj);
    return output;
}

#elif defined PS_TRIANGLE

[RootSignature(root_sig)]
float4 ps_triangle(ps_data input) : SV_Target0
{
    return float4(0.0f, 0.5f, 0.0f, 1.0f);
}

#endif
