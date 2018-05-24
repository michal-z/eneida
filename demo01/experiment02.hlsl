#define root_sig \
    "RootConstants(b0, num32BitConstants = 4)"

struct ps_data
{
    float4 position : SV_Position;
};

#if defined VS_E02_TRANSFORM

struct cb_data
{
    float2 position;
    float rotation;
    float scale;
};
ConstantBuffer<cb_data> s_cb : register(b0);

[RootSignature(root_sig)]
ps_data vs_e02_transform()
{
    ps_data output;
    output.position = float4(s_cb.position, 0.0f, 1.0f);
    return output;
}

#elif defined PS_E02_SHADE

[RootSignature(root_sig)]
float4 ps_e02_shade(ps_data input) : SV_Target0
{
    return float4(0.0f, 0.5f, 0.0f, 1.0f);
}

#endif
