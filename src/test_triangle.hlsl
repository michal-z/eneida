#define root_sig \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "CBV(b0, visibility = SHADER_VISIBILITY_VERTEX)"

struct VsIn {
    float3 position : POSITION;
};

struct PsIn {
    float4 position : SV_Position;
};

#if defined _VS

struct Const {
    float4x4 world2proj;
};
ConstantBuffer<Const> s_const : register(b0);

[RootSignature(root_sig)]
PsIn main(VsIn vin)
{
    PsIn pin;
    pin.position = mul(float4(vin.position, 1.0f), s_const.world2proj);
    return pin;
}

#elif defined _PS

[RootSignature(root_sig)]
float4 main(PsIn pin) : SV_Target0
{
    return float4(0.0f, 0.5f, 0.0f, 1.0f);
}

#endif
