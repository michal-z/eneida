#pragma once
#include "types.h"

#define k_1pi 3.141592654f
#define k_2pi 6.283185307f
#define k_1_div_1pi 0.318309886f
#define k_1_div_2pi 0.159154943f
#define k_1pi_div_2 1.570796327f
#define k_1pi_div_4 0.785398163f

typedef struct f32v3 {
    union {
        struct { f32 x, y, z; };
        f32 v[3];
    };
} f32v3;

typedef struct f32v4 {
    union {
        struct { f32 x, y, z, w; };
        f32 v[4];
    };
} f32v4;

typedef struct f32m4 {
    union {
        struct { f32v4 r0, r1, r2, r3; };
        f32v4 r[4];
        f32 m[4][4];
        f32 v[16];
    };
} f32m4;

__forceinline f32 f32_sqrt(f32 x)
{
    return sqrtf(x);
}

__forceinline f32 f32_abs(f32 x)
{
    union {
        f32 f;
        u32 u;
    } fu;
    fu.f = x;
    fu.u &= 0x7fffffff;
    return fu.f;
}

__forceinline i32 f32_equal(f32 x, f32 y, f32 epsilon)
{
    return f32_abs(x - y) <= epsilon;
}

// Implementation taken from https://github.com/Microsoft/DirectXMath
inline f32 f32_sin(f32 x)
{
    f32 quotient = k_1_div_2pi * x;
    quotient = x >= 0.0f ? (f32)((i32)(quotient + 0.5f)) : (f32)((i32)(quotient - 0.5f));

    f32 y = x - k_2pi * quotient;

    if (y > k_1pi_div_2)
        y = k_1pi - y;
    else if (y < -k_1pi_div_2)
        y = -k_1pi - y;

    f32 y2 = y * y;
    return (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;
}

// Implementation taken from https://github.com/Microsoft/DirectXMath
inline f32 f32_sin_fast(f32 x)
{
    f32 quotient = k_1_div_2pi * x;
    quotient = x >= 0.0f ? (f32)((i32)(quotient + 0.5f)) : (f32)((i32)(quotient - 0.5f));

    f32 y = x - k_2pi * quotient;

    if (y > k_1pi_div_2)
        y = k_1pi - y;
    else if (y < -k_1pi_div_2)
        y = -k_1pi - y;

    f32 y2 = y * y;
    return (((-0.00018524670f * y2 + 0.0083139502f) * y2 - 0.16665852f) * y2 + 1.0f) * y;
}

// Implementation taken from https://github.com/Microsoft/DirectXMath
inline f32 f32_cos(f32 x)
{
    f32 quotient = k_1_div_2pi * x;
    quotient = x >= 0.0f ? (f32)((i32)(quotient + 0.5f)) : (f32)((i32)(quotient - 0.5f));

    f32 y = x - k_2pi * quotient;

    f32 sign;
    if (y > k_1pi_div_2) {
        y = k_1pi - y;
        sign = -1.0f;
    } else if (y < -k_1pi_div_2) {
        y = -k_1pi - y;
        sign = -1.0f;
    } else
        sign = 1.0f;

    f32 y2 = y * y;
    f32 p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
    return sign * p;
}

// Implementation taken from https://github.com/Microsoft/DirectXMath
inline f32 f32_cos_fast(f32 x)
{
    f32 quotient = k_1_div_2pi * x;
    quotient = x >= 0.0f ? (f32)((i32)(quotient + 0.5f)) : (f32)((i32)(quotient - 0.5f));

    f32 y = x - k_2pi * quotient;

    f32 sign;
    if (y > k_1pi_div_2) {
        y = k_1pi - y;
        sign = -1.0f;
    } else if (y < -k_1pi_div_2) {
        y = -k_1pi - y;
        sign = -1.0f;
    } else
        sign = 1.0f;

    f32 y2 = y * y;
    f32 p = ((-0.0012712436f * y2 + 0.041493919f) * y2 - 0.49992746f) * y2 + 1.0f;
    return sign * p;
}

// Implementation taken from https://github.com/Microsoft/DirectXMath
inline void f32_sincos(f32 *osin, f32 *ocos, f32 x)
{
    assert(osin);
    assert(ocos);

    f32 quotient = k_1_div_2pi * x;
    quotient = x >= 0.0f ? (f32)((i32)(quotient + 0.5f)) : (f32)((i32)(quotient - 0.5f));

    f32 y = x - k_2pi * quotient;

    f32 sign;
    if (y > k_1pi_div_2) {
        y = k_1pi - y;
        sign = -1.0f;
    } else if (y < -k_1pi_div_2) {
        y = -k_1pi - y;
        sign = -1.0f;
    } else
        sign = 1.0f;

    f32 y2 = y * y;

    *osin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

    f32 p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
    *ocos = sign * p;
}

// Implementation taken from https://github.com/Microsoft/DirectXMath
inline void f32_sincos_fast(f32 *osin, f32 *ocos, f32 x)
{
    assert(osin);
    assert(ocos);

    f32 quotient = k_1_div_2pi * x;
    quotient = x >= 0.0f ? (f32)((i32)(quotient + 0.5f)) : (f32)((i32)(quotient - 0.5f));

    f32 y = x - k_2pi * quotient;

    f32 sign;
    if (y > k_1pi_div_2) {
        y = k_1pi - y;
        sign = -1.0f;
    } else if (y < -k_1pi_div_2) {
        y = -k_1pi - y;
        sign = -1.0f;
    } else
        sign = 1.0f;

    f32 y2 = y * y;

    *osin = (((-0.00018524670f * y2 + 0.0083139502f) * y2 - 0.16665852f) * y2 + 1.0f) * y;

    f32 p = ((-0.0012712436f * y2 + 0.041493919f) * y2 - 0.49992746f) * y2 + 1.0f;
    *ocos = sign * p;
}

inline f32v3 *f32v3_set(f32v3 *out, f32 x, f32 y, f32 z)
{
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

inline f32v3 *f32v3_add(f32v3 *out, const f32v3 *a, const f32v3 *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
    return out;
}

inline f32v3 *f32v3_sub(f32v3 *out, const f32v3 *a, const f32v3 *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
    return out;
}

inline f32v3 *f32v3_neg(f32v3 *out, const f32v3 *a)
{
    out->x = -a->x;
    out->y = -a->y;
    out->z = -a->z;
    return out;
}

inline f32v4 *f32v4_set(f32v4 *out, f32 x, f32 y, f32 z, f32 w)
{
    out->x = x;
    out->y = y;
    out->z = z;
    out->w = w;
    return out;
}

inline f32 f32v3_dot(const f32v3 *a, const f32v3 *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

inline f32v3 *f32v3_cross(f32v3 *out, const f32v3 *a, const f32v3 *b)
{
    f32 x = a->y * b->z - a->z * b->y;
    f32 y = a->z * b->x - a->x * b->z;
    f32 z = a->x * b->y - a->y * b->x;
    return f32v3_set(out, x, y, z);
}

inline f32 f32v3_length(const f32v3 *a)
{
    return f32_sqrt(f32v3_dot(a, a));
}

inline f32v3 *f32v3_normalize(f32v3 *out, const f32v3 *a)
{
    f32 length = f32v3_length(a);
    assert(!f32_equal(length, 0.0f, 0.0001f));
    f32 rcplen = 1.0f / length;
    return f32v3_set(out, rcplen * a->x, rcplen * a->y, rcplen * a->z);
}

inline f32m4 *f32m4_fovperspective(f32m4 *out, f32 fovy, f32 aspect, f32 n, f32 f)
{
    assert(n > 0.0f && f > 0.0f);
    assert(!f32_equal(fovy, 0.0f, 0.00001f * 2.0f));
    assert(!f32_equal(aspect, 0.0f, 0.00001f));
    assert(!f32_equal(f, n, 0.00001f));

    f32 sinfov, cosfov;
    f32_sincos(&sinfov, &cosfov, 0.5f * fovy);

    f32 h = cosfov / sinfov;
    f32 w = h / aspect;
    f32 r = f / (f - n);

    f32v4_set(&out->r0, w, 0.0f, 0.0f, 0.0f);
    f32v4_set(&out->r1, 0.0f, h, 0.0f, 0.0f);
    f32v4_set(&out->r2, 0.0f, 0.0f, r, 1.0f);
    f32v4_set(&out->r3, 0.0f, 0.0f, -r * n, 0.0f);

    return out;
}

inline f32m4 *f32m4_transpose(f32m4 *out, const f32m4 *m)
{
    f32m4 r;
    r.m[0][0] = m->m[0][0];
    r.m[1][0] = m->m[0][1];
    r.m[2][0] = m->m[0][2];
    r.m[3][0] = m->m[0][3];
    r.m[0][1] = m->m[1][0];
    r.m[1][1] = m->m[1][1];
    r.m[2][1] = m->m[1][2];
    r.m[3][1] = m->m[1][3];
    r.m[0][2] = m->m[2][0];
    r.m[1][2] = m->m[2][1];
    r.m[2][2] = m->m[2][2];
    r.m[3][2] = m->m[2][3];
    r.m[0][3] = m->m[3][0];
    r.m[1][3] = m->m[3][1];
    r.m[2][3] = m->m[3][2];
    r.m[3][3] = m->m[3][3];
    *out = r;
    return out;
}

inline f32m4 *f32m4_mul(f32m4 *out, const f32m4 *m0, const f32m4 *m1)
{
    f32m4 r;
    r.m[0][0] = m0->m[0][0] * m1->m[0][0] + m0->m[0][1] * m1->m[1][0] + m0->m[0][2] * m1->m[2][0] + m0->m[0][3] * m1->m[3][0];
    r.m[0][1] = m0->m[0][0] * m1->m[0][1] + m0->m[0][1] * m1->m[1][1] + m0->m[0][2] * m1->m[2][1] + m0->m[0][3] * m1->m[3][1];
    r.m[0][2] = m0->m[0][0] * m1->m[0][2] + m0->m[0][1] * m1->m[1][2] + m0->m[0][2] * m1->m[2][2] + m0->m[0][3] * m1->m[3][2];
    r.m[0][3] = m0->m[0][0] * m1->m[0][3] + m0->m[0][1] * m1->m[1][3] + m0->m[0][2] * m1->m[2][3] + m0->m[0][3] * m1->m[3][3];
    r.m[1][0] = m0->m[1][0] * m1->m[0][0] + m0->m[1][1] * m1->m[1][0] + m0->m[1][2] * m1->m[2][0] + m0->m[1][3] * m1->m[3][0];
    r.m[1][1] = m0->m[1][0] * m1->m[0][1] + m0->m[1][1] * m1->m[1][1] + m0->m[1][2] * m1->m[2][1] + m0->m[1][3] * m1->m[3][1];
    r.m[1][2] = m0->m[1][0] * m1->m[0][2] + m0->m[1][1] * m1->m[1][2] + m0->m[1][2] * m1->m[2][2] + m0->m[1][3] * m1->m[3][2];
    r.m[1][3] = m0->m[1][0] * m1->m[0][3] + m0->m[1][1] * m1->m[1][3] + m0->m[1][2] * m1->m[2][3] + m0->m[1][3] * m1->m[3][3];
    r.m[2][0] = m0->m[2][0] * m1->m[0][0] + m0->m[2][1] * m1->m[1][0] + m0->m[2][2] * m1->m[2][0] + m0->m[2][3] * m1->m[3][0];
    r.m[2][1] = m0->m[2][0] * m1->m[0][1] + m0->m[2][1] * m1->m[1][1] + m0->m[2][2] * m1->m[2][1] + m0->m[2][3] * m1->m[3][1];
    r.m[2][2] = m0->m[2][0] * m1->m[0][2] + m0->m[2][1] * m1->m[1][2] + m0->m[2][2] * m1->m[2][2] + m0->m[2][3] * m1->m[3][2];
    r.m[2][3] = m0->m[2][0] * m1->m[0][3] + m0->m[2][1] * m1->m[1][3] + m0->m[2][2] * m1->m[2][3] + m0->m[2][3] * m1->m[3][3];
    r.m[3][0] = m0->m[3][0] * m1->m[0][0] + m0->m[3][1] * m1->m[1][0] + m0->m[3][2] * m1->m[2][0] + m0->m[3][3] * m1->m[3][0];
    r.m[3][1] = m0->m[3][0] * m1->m[0][1] + m0->m[3][1] * m1->m[1][1] + m0->m[3][2] * m1->m[2][1] + m0->m[3][3] * m1->m[3][1];
    r.m[3][2] = m0->m[3][0] * m1->m[0][2] + m0->m[3][1] * m1->m[1][2] + m0->m[3][2] * m1->m[2][2] + m0->m[3][3] * m1->m[3][2];
    r.m[3][3] = m0->m[3][0] * m1->m[0][3] + m0->m[3][1] * m1->m[1][3] + m0->m[3][2] * m1->m[2][3] + m0->m[3][3] * m1->m[3][3];
    *out = r;
    return out;
}

inline f32m4 *f32m4_identity(f32m4 *out)
{
    f32v4_set(&out->r0, 1.0f, 0.0f, 0.0f, 0.0f);
    f32v4_set(&out->r1, 0.0f, 1.0f, 0.0f, 0.0f);
    f32v4_set(&out->r2, 0.0f, 0.0f, 1.0f, 0.0f);
    f32v4_set(&out->r3, 0.0f, 0.0f, 0.0f, 1.0f);
    return out;
}

inline f32m4 *f32m4_translation(f32m4 *out, f32 x, f32 y, f32 z)
{
    f32v4_set(&out->r0, 1.0f, 0.0f, 0.0f, 0.0f);
    f32v4_set(&out->r1, 0.0f, 1.0f, 0.0f, 0.0f);
    f32v4_set(&out->r2, 0.0f, 0.0f, 1.0f, 0.0f);
    f32v4_set(&out->r3, x, y, z, 1.0f);
    return out;
}

inline f32m4 *f32m4_rotation_ay(f32m4 *out, f32 radians)
{
    f32 sinv, cosv;
    f32_sincos(&sinv, &cosv, radians);
    f32v4_set(&out->r0, cosv, 0.0f, -sinv, 0.0f);
    f32v4_set(&out->r1, 0.0f, 1.0f, 0.0f, 0.0f);
    f32v4_set(&out->r2, sinv, 0.0f, cosv, 0.0f);
    f32v4_set(&out->r3, 0.0f, 0.0f, 0.0f, 1.0f);
    return out;
}

inline f32m4 *f32m4_look_at(f32m4 *out, const f32v3 *eye, const f32v3 *at, const f32v3 *up)
{
    f32v3 ax, ay, az;
    f32v3_normalize(&az, f32v3_sub(&az, at, eye));
    f32v3_normalize(&ax, f32v3_cross(&ax, up, &az));
    f32v3_cross(&ay, &az, &ax);
    f32v4_set(&out->r0, ax.x, ay.x, az.x, 0.0f);
    f32v4_set(&out->r1, ax.y, ay.y, az.y, 0.0f);
    f32v4_set(&out->r2, ax.z, ay.z, az.z, 0.0f);
    f32v4_set(&out->r3, -f32v3_dot(&ax, eye), -f32v3_dot(&ay, eye), -f32v3_dot(&az, eye), 1.0f);
    return out;
}
