#define root_sig \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)"

struct VsIn {
    float3 position : POSITION;
};

struct PsIn {
    float4 position : SV_Position;
};

#if defined _VS

[RootSignature(root_sig)]
PsIn vs_main(VsIn vin)
{
    PsIn pin;
    pin.position = float4(vin.position, 1.0f);
    return pin;
}

#elif defined _PS

[RootSignature(root_sig)]
float4 ps_main(PsIn pin) : SV_Target0
{
    return float4(0.0f, 0.5f, 0.0f, 1.0f);
}

#endif
