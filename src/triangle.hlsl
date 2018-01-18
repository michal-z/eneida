#if defined _VS

[RootSignature("")]
float4 vs_main(uint id : SV_VertexID) : SV_Position
{
    float2 v[3] = { float2(-1.0f, -1.0f), float2(1.0f, -1.0f), float2(0.0f, 1.0f) };
    return float4(v[id], 0.0f, 1.0f);
}

#elif defined _PS

float4 ps_main(float4 pos : SV_Position) : SV_Target0
{
    return float4(0.0f, 0.5f, 0.0f, 1.0f);
}

#endif
