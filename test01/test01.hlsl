//-------------------------------------------------------------------------------------------------
#if defined(VS_0) || defined(PS_0)
//-------------------------------------------------------------------------------------------------

#define Rsi \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)"
    //"CBV(b0, visibility = SHADER_VISIBILITY_VERTEX)"

struct PsData {
    float4 position : SV_Position;
};

#if defined(VS_0)

struct VsData {
    float3 position : POSITION;
};
struct CbData {
    float4x4 objectToProj;
};
ConstantBuffer<CbData> GCb;

[RootSignature(Rsi)]
PsData VertexMain(VsData input) : SV_Position
{
    PsData output;
    output.position = float4(input.position, 1.0f);//mul(float4(input.position, 1.0f), GCb.objectToProj);
    return output;
}

#elif defined(PS_0)

[RootSignature(Rsi)]
float4 PixelMain(PsData input) : SV_Target0
{
    return float4(0.0f, 0.5f, 0.0f, 0.0f);
}

#endif
//-------------------------------------------------------------------------------------------------
#endif
