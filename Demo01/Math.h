#pragma once

#define k_Pi 3.141592654f
#define k_2Pi 6.283185307f
#define k_1DivPi 0.318309886f
#define k_1Div2Pi 0.159154943f
#define k_PiDiv2 1.570796327f
#define k_PiDiv4 0.785398163f

typedef union __declspec(intrin_type) __declspec(align(16)) __m128
{
    float               m128_f32[4];
    unsigned __int64    m128_u64[2];
    __int8              m128_i8[16];
    __int16             m128_i16[8];
    __int32             m128_i32[4];
    __int64             m128_i64[2];
    unsigned __int8     m128_u8[16];
    unsigned __int16    m128_u16[8];
    unsigned __int32    m128_u32[4];
} __m128;

extern __m128 _mm_sqrt_ss(__m128 _A);
extern __m128 _mm_load_ss(float const*_A);
extern void _mm_store_ss(float *_V, __m128 _A);

typedef struct f32vec3
{
    union {
        struct { f32 X, Y, Z; };
        f32 V[3];
    };
} f32vec3;

typedef struct f32vec4
{
    union {
        struct { f32 X, Y, Z, W; };
        f32 V[4];
    };
} f32vec4;

typedef struct f32mat4
{
    union {
        struct { f32vec4 C0, C1, C2, C3; };
        f32vec4 C[4];
        f32 M[4][4];
        f32 V[16];
    };
} f32mat4;

inline u32
U32Rand(u32 *State)
{
    Assert(State);
    *State = *State * 1103515245 + 12345;
    return (*State >> 16) & 0x7fff;
}

inline f32
F32Rand(u32 *State)
{
    u32 Result = (127 << 23) | (U32Rand(State) << 8);
    return *(f32 *)&Result - 1.0f;
}

inline f32
F32RandRange(u32 *State, f32 Begin, f32 End)
{
    Assert(Begin < End);
    return Begin + (End - Begin) * F32Rand(State);
}

__forceinline f32
F32Sqrt(f32 X)
{
    __m128 Xv = _mm_load_ss(&X);
    Xv = _mm_sqrt_ss(Xv);
    _mm_store_ss(&X, Xv);
    return X;
}

__forceinline f32
F32Abs(f32 X)
{
    union { f32 F; u32 U; } Fu;
    Fu.F = X;
    Fu.U &= 0x7fffffff;
    return Fu.F;
}

__forceinline i32
F32Equal(f32 X, f32 Y, f32 Epsilon)
{
    return (F32Abs(X) - F32Abs(Y)) <= Epsilon;
}

__forceinline f32
F32PreSin(f32 X)
{
    f32 Quotient = k_1Div2Pi * X;
    Quotient = X >= 0.0f ? (f32)((i32)(Quotient + 0.5f)) : (f32)((i32)(Quotient - 0.5f));

    f32 Y = X - k_2Pi * Quotient;

    if (Y > k_PiDiv2)
        Y = k_Pi - Y;
    else if (Y < -k_PiDiv2)
        Y = -k_Pi - Y;

    return Y;
}

__forceinline f32
F32PreCos(f32 X, f32 *Sign)
{
    Assert(Sign);

    f32 Quotient = k_1Div2Pi * X;
    Quotient = X >= 0.0f ? (f32)((i32)(Quotient + 0.5f)) : (f32)((i32)(Quotient - 0.5f));

    f32 Y = X - k_2Pi * Quotient;

    if (Y > k_PiDiv2)
    {
        Y = k_Pi - Y;
        *Sign = -1.0f;
    }
    else if (Y < -k_PiDiv2)
    {
        Y = -k_Pi - Y;
        *Sign = -1.0f;
    }
    else
        *Sign = 1.0f;

    return Y;
}

inline f32
F32Sin(f32 X)
{
    f32 Y = F32PreSin(X);
    f32 Y2 = Y * Y;
    return (((((-2.3889859e-08f * Y2 + 2.7525562e-06f) * Y2 - 0.00019840874f) * Y2 + 0.0083333310f) * Y2 - 0.16666667f) * Y2 + 1.0f) * Y;
}

inline f32
F32SinFast(f32 X)
{
    f32 Y = F32PreSin(X);
    f32 Y2 = Y * Y;
    return (((-0.00018524670f * Y2 + 0.0083139502f) * Y2 - 0.16665852f) * Y2 + 1.0f) * Y;
}

inline f32
F32Cos(f32 X)
{
    f32 Sign;
    f32 Y = F32PreCos(X, &Sign);
    f32 Y2 = Y * Y;
    return Sign * (((((-2.6051615e-07f * Y2 + 2.4760495e-05f) * Y2 - 0.0013888378f) * Y2 + 0.041666638f) * Y2 - 0.5f) * Y2 + 1.0f);
}

inline f32
F32CosFast(f32 X)
{
    f32 Sign;
    f32 Y = F32PreCos(X, &Sign);
    f32 Y2 = Y * Y;
    return Sign * (((-0.0012712436f * Y2 + 0.041493919f) * Y2 - 0.49992746f) * Y2 + 1.0f);
}

inline void
F32SinCos(f32 X, f32 *Sin, f32 *Cos)
{
    Assert(Sin);
    Assert(Cos);

    f32 Sign;
    f32 Y = F32PreCos(X, &Sign);
    f32 Y2 = Y * Y;
    *Sin = (((((-2.3889859e-08f * Y2 + 2.7525562e-06f) * Y2 - 0.00019840874f) * Y2 + 0.0083333310f) * Y2 - 0.16666667f) * Y2 + 1.0f) * Y;
    *Cos = Sign * (((((-2.6051615e-07f * Y2 + 2.4760495e-05f) * Y2 - 0.0013888378f) * Y2 + 0.041666638f) * Y2 - 0.5f) * Y2 + 1.0f);
}

inline void
F32SinCosFast(f32 X, f32 *Sin, f32 *Cos)
{
    Assert(Sin);
    Assert(Cos);

    f32 Sign;
    f32 Y = F32PreCos(X, &Sign);
    f32 Y2 = Y * Y;
    *Sin = (((-0.00018524670f * Y2 + 0.0083139502f) * Y2 - 0.16665852f) * Y2 + 1.0f) * Y;
    *Cos = Sign * (((-0.0012712436f * Y2 + 0.041493919f) * Y2 - 0.49992746f) * Y2 + 1.0f);
}
