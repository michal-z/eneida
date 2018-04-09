#pragma once
#include "types.h"

#define k_1pi 3.141592654f
#define k_2pi 6.283185307f
#define k_1_div_1pi 0.318309886f
#define k_1_div_2pi 0.159154943f
#define k_1pi_div_2 1.570796327f
#define k_1pi_div_4 0.785398163f

typedef struct Vec3 {
    union {
        struct { float x, y, z; };
        float m[3];
    };
} Vec3;

typedef struct Vec4 {
    union {
        struct { float x, y, z, w; };
        float m[4];
    };
} Vec4;

typedef struct Mat4 {
    union {
        struct { Vec4 r0, r1, r2, r3; };
        float m[4][4];
    };
} Mat4;

float sqrtf(float x);

inline i32 scalar_near_equal(float s0, float s1, float epsilon)
{
    float d = s0 - s1;
    d = d < 0.0f ? -d : d; // fabsf
    return d <= epsilon;
}

// Implementation taken from https://github.com/Microsoft/DirectXMath
inline float sin1f(float x)
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
inline float sin1f_fast(float x)
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
inline float cos1f(float x)
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
inline float cos1f_fast(float x)
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

// Implementation taken from https://github.com/Microsoft/DirectXMath
inline void sincos1f(float x, float *osin, float *ocos)
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

// Implementation taken from https://github.com/Microsoft/DirectXMath
inline void sincos1f_fast(float x, float *osin, float *ocos)
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

inline Vec3 *vec3_set(float x, float y, float z, Vec3 *out)
{
    out->x = x;
    out->y = y;
    out->z = z;
    return out;
}

inline Vec3 *vec3_sub(const Vec3 *v0, const Vec3 *v1, Vec3 *out)
{
    out->x = v0->x - v1->x;
    out->y = v0->y - v1->y;
    out->z = v0->z - v1->z;
    return out;
}

inline Vec3 *vec3_neg(const Vec3 *v, Vec3 *out)
{
    out->x = -v->x;
    out->y = -v->y;
    out->z = -v->z;
    return out;
}

inline Vec4 *vec4_set(float x, float y, float z, float w, Vec4 *out)
{
    out->x = x;
    out->y = y;
    out->z = z;
    out->w = w;
    return out;
}

inline float vec3_dot(const Vec3 *v0, const Vec3 *v1)
{
    return v0->x * v1->x + v0->y * v1->y + v0->z * v1->z;
}

inline Vec3 *vec3_cross(const Vec3 *v0, const Vec3 *v1, Vec3 *out)
{
    float x = v0->y * v1->z - v0->z * v1->y;
    float y = v0->z * v1->x - v0->x * v1->z;
    float z = v0->x * v1->y - v0->y * v1->x;
    return vec3_set(x, y, z, out);
}

inline float vec3_length(const Vec3 *v)
{
    return sqrtf(vec3_dot(v, v));
}

inline Vec3 *vec3_normalize(const Vec3 *v, Vec3 *out)
{
    float length = vec3_length(v);
    assert(!scalar_near_equal(length, 0.0f, 0.00001f));
    float rlength = 1.0f / length;
    return vec3_set(rlength * v->x, rlength * v->y, rlength * v->z, out);
}

inline Mat4 *mat4_fovperspective(float fovy, float aspect, float n, float f, Mat4 *out)
{
    assert(n > 0.0f && f > 0.0f);
    assert(!scalar_near_equal(fovy, 0.0f, 0.00001f * 2.0f));
    assert(!scalar_near_equal(aspect, 0.0f, 0.00001f));
    assert(!scalar_near_equal(f, n, 0.00001f));

    float sin_fov;
    float cos_fov;
    sincos1f(0.5f * fovy, &sin_fov, &cos_fov);

    float h = cos_fov / sin_fov;
    float w = h / aspect;
    float r = f / (f - n);

    vec4_set(w, 0.0f, 0.0f, 0.0f, &out->r0);
    vec4_set(0.0f, h, 0.0f, 0.0f, &out->r1);
    vec4_set(0.0f, 0.0f, r, 1.0f, &out->r2);
    vec4_set(0.0f, 0.0f, -r * n, 0.0f, &out->r3);

    return out;
}

inline Mat4 *mat4_transpose(const Mat4 *m, Mat4 *out)
{
    Mat4 r;
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

inline Mat4 *mat4_mul(const Mat4 *m0, const Mat4 *m1, Mat4 *out)
{
    Mat4 r;
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

inline Mat4 *mat4_identity(Mat4 *out)
{
    vec4_set(1.0f, 0.0f, 0.0f, 0.0f, &out->r0);
    vec4_set(0.0f, 1.0f, 0.0f, 0.0f, &out->r1);
    vec4_set(0.0f, 0.0f, 1.0f, 0.0f, &out->r2);
    vec4_set(0.0f, 0.0f, 0.0f, 1.0f, &out->r3);
    return out;
}

inline Mat4 *mat4_translation(float x, float y, float z, Mat4 *out)
{
    vec4_set(1.0f, 0.0f, 0.0f, 0.0f, &out->r0);
    vec4_set(0.0f, 1.0f, 0.0f, 0.0f, &out->r1);
    vec4_set(0.0f, 0.0f, 1.0f, 0.0f, &out->r2);
    vec4_set(x, y, z, 1.0f, &out->r3);
    return out;
}

inline Mat4 *mat4_rotation_ay(float arad, Mat4 *out)
{
    float sinv, cosv;
    sincos1f(arad, &sinv, &cosv);
    vec4_set(cosv, 0.0f, -sinv, 0.0f, &out->r0);
    vec4_set(0.0f, 1.0f, 0.0f, 0.0f, &out->r1);
    vec4_set(sinv, 0.0f, cosv, 0.0f, &out->r2);
    vec4_set(0.0f, 0.0f, 0.0f, 1.0f, &out->r3);
    return out;
}

inline Mat4 *mat4_look_at(const Vec3 *eye, const Vec3 *at, const Vec3 *up, Mat4 *out)
{
    Vec3 ax, ay, az;
    vec3_normalize(vec3_sub(at, eye, &az), &az);
    vec3_normalize(vec3_cross(up, &az, &ax), &ax);
    vec3_cross(&az, &ax, &ay);

    vec4_set(ax.x, ay.x, az.x, 0.0f, &out->r0);
    vec4_set(ax.y, ay.y, az.y, 0.0f, &out->r1);
    vec4_set(ax.z, ay.z, az.z, 0.0f, &out->r2);
    vec4_set(-vec3_dot(&ax, eye), -vec3_dot(&ay, eye), -vec3_dot(&az, eye), 1.0f, &out->r3);

    return out;
}
