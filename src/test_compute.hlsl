#if defined _VS

float4 main(uint id : SV_VertexID) : SV_Position
{
    float2 v[3] = { float2(-1.0f, -1.0f), float2(3.0f, -1.0f), float2(-1.0f, 3.0f) };
    return float4(v[id], 0.0f, 1.0f);
}

#elif defined _PS

#define root_sig \
    "DescriptorTable(SRV(t0), visibility = SHADER_VISIBILITY_PIXEL)"

Texture2D<float4> s_texture : register(t0);

[RootSignature(root_sig)]
float4 main(float4 pos : SV_Position) : SV_Target0
{
    return s_texture[pos.xy];
}

#elif defined _CS

#define root_sig \
    "DescriptorTable(UAV(u0))"

RWTexture2D<float4> s_target : register(u0);

float2 complex_mul(float2 a, float2 b)
{
    return float2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}

float2 complex_sq(float2 a)
{
    return float2(a.x * a.x - a.y * a.y, 2 * a.x * a.y);
}

[numthreads(8, 8, 1)]
[RootSignature(root_sig)]
void main(uint3 tid : SV_DispatchThreadID)
{
    float2 z = float2(0.0f, 0.0f);
    float2 dz = float2(1.0f, 0.0f);
    float d = 0.0f;

    float2 c = -1.0f + 2.0f * float2(tid.x, 1024.0f - tid.y) / float2(1024.0f, 1024.0f);
    c.x -= 0.5f;

    for (int i = 0; i < 256; ++i) {
        dz = 2.0f * complex_mul(z, dz) + float2(1.0f, 0.0f);
        z = complex_sq(z) + c;

        float m2 = dot(z, z);
        if (m2 > 100.0f) {
            d = sqrt(m2 / dot(dz, dz)) * log(m2);
            break;
        }
    }

    d = pow(abs(d), 0.3f);
    s_target[tid.xy] = float4(d, d, d, 1.0f);
}

#endif
