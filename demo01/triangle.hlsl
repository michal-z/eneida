#define root_sig \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX)"

struct PixelData {
    float4 position : SV_Position;
};

#if defined TRIANGLE_VS

struct VertexData {
    float3 position : POSITION;
};

struct ConstData {
    float4x4 world2proj;
};
ConstantBuffer<ConstData> s_cb : register(b0);

[RootSignature(root_sig)]
PixelData TriangleVs(VertexData input)
{
    PixelData output;
    output.position = mul(float4(input.position, 1.0f), s_cb.world2proj);
    return output;
}

#elif defined TRIANGLE_PS

[RootSignature(root_sig)]
float4 TrianglePs(PixelData input) : SV_Target0
{
    return float4(0.0f, 0.5f, 0.0f, 1.0f);
}

#endif
