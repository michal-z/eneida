#pragma once

#include "library.h"

#define k_1pi 3.141592654f
#define k_2pi 6.283185307f
#define k_1_div_1pi 0.318309886f
#define k_1_div_2pi 0.159154943f
#define k_1pi_div_2 1.570796327f
#define k_1pi_div_4 0.785398163f

typedef struct Vec3 {
    union {
        struct {
            float x, y, z;
        };
        float m[3];
    };
} Vec3;

typedef struct Vec4 {
    union {
        struct {
            float x, y, z, w;
        };
        float m[4];
    };
} Vec4;

typedef struct Mat4 {
    union {
        struct {
            Vec4 c0, c1, c2, c3;
        };
        float m[4][4];
    };
} Mat4;

static inline Vec3 *vec3_set(Vec3 *out, float x, float y, float z)
{
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

static inline Vec3 *vec3_mul(Vec3 *out, const Vec3 *v0, const Vec3 *v1)
{
    out->x = v0->x * v1->x;
    out->y = v0->y * v1->y;
    out->z = v0->z * v1->z;
    return out;
}

static inline Mat4 mat4_perspective(float fovy, float aspect, float n, float f)
{
    assert(0);
    float k = fovy;//tanf(k_pi_div_2 - 0.5f * fovy);
    float ri = 1.0f / (n - f);
    Mat4 m = {
        k / aspect, 0.0f, 0.0f, 0.0f,
        0.0f, k, 0.0f, 0.0f,
        0.0f, 0.0f, (n + f) * ri, -1.0f,
        0.0f, 0.0f, n * f * ri * 2.0f, 0.0f
    };
    return m;
}
/*
static mat4 LookAt(const vec3 &eye, const vec3 &at, const vec3 &up) {
    const vec3 az = normalize(eye - at);
    const vec3 ax = normalize(cross(up, az));
    const vec3 ay = cross(az, ax);
    return mat4(ax[0], ay[0], az[0], 0.0f,
        ax[1], ay[1], az[1], 0.0f,
        ax[2], ay[2], az[2], 0.0f,
        -dot(ax, eye), -dot(ay, eye), -dot(az, eye), 1.0f);
}
*/

// Implementation taken from https://github.com/Microsoft/DirectXMath
inline float sinf(float x)
{
    float quotient = k_1_div_2pi * x;
    quotient = x >= 0.0f ? (float)((i32)(quotient + 0.5f)) : (float)((i32)(quotient - 0.5f));

    float y = x - k_2pi * quotient;

    if (y > k_1pi_div_2)
        y = k_1pi - y;
    else if (y < -k_1pi_div_2)
        y = -k_1pi - y;

    float y2 = y * y;
    return (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;
}

// Implementation taken from https://github.com/Microsoft/DirectXMath
inline float sinf_fast(float x)
{
    float quotient = k_1_div_2pi * x;
    quotient = x >= 0.0f ? (float)((i32)(quotient + 0.5f)) : (float)((i32)(quotient - 0.5f));

    float y = x - k_2pi * quotient;

    if (y > k_1pi_div_2)
        y = k_1pi - y;
    else if (y < -k_1pi_div_2)
        y = -k_1pi - y;

    float y2 = y * y;
    return (((-0.00018524670f * y2 + 0.0083139502f) * y2 - 0.16665852f) * y2 + 1.0f) * y;
}

// Implementation taken from https://github.com/Microsoft/DirectXMath
inline float cosf(float x)
{
    float quotient = k_1_div_2pi * x;
    quotient = x >= 0.0f ? (float)((i32)(quotient + 0.5f)) : (float)((i32)(quotient - 0.5f));

    float y = x - k_2pi * quotient;

    float sign;
    if (y > k_1pi_div_2) {
        y = k_1pi - y;
        sign = -1.0f;
    } else if (y < -k_1pi_div_2) {
        y = -k_1pi - y;
        sign = -1.0f;
    } else
        sign = 1.0f;

    float y2 = y * y;
    float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
    return sign * p;
}

// Implementation taken from https://github.com/Microsoft/DirectXMath
inline float cosf_fast(float x)
{
    float quotient = k_1_div_2pi * x;
    quotient = x >= 0.0f ? (float)((i32)(quotient + 0.5f)) : (float)((i32)(quotient - 0.5f));

    float y = x - k_2pi * quotient;

    float sign;
    if (y > k_1pi_div_2) {
        y = k_1pi - y;
        sign = -1.0f;
    } else if (y < -k_1pi_div_2) {
        y = -k_1pi - y;
        sign = -1.0f;
    } else
        sign = 1.0f;

    float y2 = y * y;
    float p = ((-0.0012712436f * y2 + 0.041493919f) * y2 - 0.49992746f) * y2 + 1.0f;
    return sign * p;
}

inline void sincosf(float *osin, float *ocos, float x)
{
    assert(osin);
    assert(ocos);

    float quotient = k_1_div_2pi * x;
    quotient = x >= 0.0f ? (float)((i32)(quotient + 0.5f)) : (float)((i32)(quotient - 0.5f));

    float y = x - k_2pi * quotient;

    float sign;
    if (y > k_1pi_div_2) {
        y = k_1pi - y;
        sign = -1.0f;
    } else if (y < -k_1pi_div_2) {
        y = -k_1pi - y;
        sign = -1.0f;
    } else
        sign = 1.0f;

    float y2 = y * y;

    *osin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

    float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
    *ocos = sign * p;
}

inline void sincosf_fast(float *osin, float *ocos, float x)
{
    assert(osin);
    assert(ocos);

    float quotient = k_1_div_2pi * x;
    quotient = x >= 0.0f ? (float)((i32)(quotient + 0.5f)) : (float)((i32)(quotient - 0.5f));

    float y = x - k_2pi * quotient;

    float sign;
    if (y > k_1pi_div_2) {
        y = k_1pi - y;
        sign = -1.0f;
    } else if (y < -k_1pi_div_2) {
        y = -k_1pi - y;
        sign = -1.0f;
    } else
        sign = 1.0f;

    float y2 = y * y;

    *osin = (((-0.00018524670f * y2 + 0.0083139502f) * y2 - 0.16665852f) * y2 + 1.0f) * y;

    float p = ((-0.0012712436f * y2 + 0.041493919f) * y2 - 0.49992746f) * y2 + 1.0f;
    *ocos = sign * p;
}
