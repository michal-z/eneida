#define GRootSignature \
    "RootConstants(b0, num32BitConstants = 2), " \
    "DescriptorTable(SRV(t0), UAV(u0, numDescriptors = 4))"

Texture2D<float4> GSrcMipmap : register(t0);
RWTexture2D<float4> GOutMipmap1 : register(u0);
RWTexture2D<float4> GOutMipmap2 : register(u1);
RWTexture2D<float4> GOutMipmap3 : register(u2);
RWTexture2D<float4> GOutMipmap4 : register(u3);

struct FParams
{
    uint SrcMipLevel;
    uint NumMipLevels;
};
ConstantBuffer<FParams> GParams : register(b0);

groupshared float GRed[64];
groupshared float GGreen[64];
groupshared float GBlue[64];
groupshared float GAlpha[64];

void StoreColor(uint Idx, float4 Color)
{
    GRed[Idx] = Color.r;
    GGreen[Idx] = Color.g;
    GBlue[Idx] = Color.b;
    GAlpha[Idx] = Color.a;
}

float4 LoadColor(uint Idx)
{
    return float4(GRed[Idx], GGreen[Idx], GBlue[Idx], GAlpha[Idx]);
}

[RootSignature(GRootSignature)]
[numthreads(8, 8, 1)]
void main_cs(uint3 GlobalID : SV_DispatchThreadID, uint GroupIdx : SV_GroupIndex)
{
    uint x = GlobalID.x * 2;
    uint y = GlobalID.y * 2;

    float4 S00 = GSrcMipmap.mips[GParams.SrcMipLevel][uint2(x, y)];
    float4 S10 = GSrcMipmap.mips[GParams.SrcMipLevel][uint2(x + 1, y)];
    float4 S01 = GSrcMipmap.mips[GParams.SrcMipLevel][uint2(x, y + 1)];
    float4 S11 = GSrcMipmap.mips[GParams.SrcMipLevel][uint2(x + 1, y + 1)];
    S00 = 0.25f * (S00 + S01 + S10 + S11);

    GOutMipmap1[GlobalID.xy] = S00;
    StoreColor(GroupIdx, S00);

    if (GParams.NumMipLevels == 1)
    {
        return;
    }
    GroupMemoryBarrierWithGroupSync();

    if ((GroupIdx & 0x9) == 0)
    {
        S10 = LoadColor(GroupIdx + 1);
        S01 = LoadColor(GroupIdx + 8);
        S11 = LoadColor(GroupIdx + 9);
        S00 = 0.25f * (S00 + S01 + S10 + S11);

        GOutMipmap2[GlobalID.xy / 2] = S00;
        StoreColor(GroupIdx, S00);
    }

    if (GParams.NumMipLevels == 2)
    {
        return;
    }
    GroupMemoryBarrierWithGroupSync();

    if ((GroupIdx & 0x1b) == 0)
    {
        S10 = LoadColor(GroupIdx + 2);
        S01 = LoadColor(GroupIdx + 16);
        S11 = LoadColor(GroupIdx + 18);
        S00 = 0.25f * (S00 + S01 + S10 + S11);

        GOutMipmap3[GlobalID.xy / 4] = S00;
        StoreColor(GroupIdx, S00);
    }

    if (GParams.NumMipLevels == 3)
    {
        return;
    }
    GroupMemoryBarrierWithGroupSync();

    if (GroupIdx == 0)
    {
        S10 = LoadColor(GroupIdx + 4);
        S01 = LoadColor(GroupIdx + 32);
        S11 = LoadColor(GroupIdx + 36);
        S00 = 0.25f * (S00 + S01 + S10 + S11);

        GOutMipmap4[GlobalID.xy / 8] = S00;
        StoreColor(GroupIdx, S00);
    }
}
