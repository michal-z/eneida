//-------------------------------------------------------------------------------------------------
#if defined(VS_0) || defined(PS_0)
//-------------------------------------------------------------------------------------------------

#define Rsi \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT)"

#if defined(VS_0)

[RootSignature(Rsi)]
float4 VertexMain(uint vertexIndex : SV_VertexID) : SV_Position
{
    float2 vertices[] = { float2(-1.0f, -1.0f), float2(0.0f, 1.0f), float2(1.0f, -1.0f) };
    return float4(vertices[vertexIndex], 0.0f, 1.0f);
}

#elif defined(PS_0)

[RootSignature(Rsi)]
float4 PixelMain(float4 position : SV_Position) : SV_Target0
{
    return float4(0.0f, 0.5f, 0.0f, 0.0f);
}

#endif
//-------------------------------------------------------------------------------------------------
#endif
