#ifndef MZ_MATH_INCLUDED_
#define MZ_MATH_INCLUDED_

#include "mz_fwd.h"

typedef float __v4sf __attribute__((__vector_size__(16)));
typedef float __m128 __attribute__((__vector_size__(16), __aligned__(16)));

#define __DEFAULT_FN_ATTRS                                                                         \
  __attribute__((__always_inline__, __nodebug__, __target__("sse"), __min_vector_width__(128)))

static __inline__ __m128 __DEFAULT_FN_ATTRS _mm_sqrt_ss(__m128 __a) {
  return (__m128)__builtin_ia32_sqrtss((__v4sf)__a);
}

static __inline__ void __DEFAULT_FN_ATTRS _mm_store_ss(float *__p, __m128 __a) {
  struct __mm_store_ss_struct {
    float __u;
  } __attribute__((__packed__, __may_alias__));
  ((struct __mm_store_ss_struct *)__p)->__u = __a[0];
}

static __inline__ __m128 __DEFAULT_FN_ATTRS _mm_load_ss(const float *__p) {
  struct __mm_load_ss_struct {
    float __u;
  } __attribute__((__packed__, __may_alias__));
  float __u = ((const struct __mm_load_ss_struct *)__p)->__u;
  return __extension__(__m128){__u, 0, 0, 0};
}

inline f32 __attribute__((__always_inline__, __nodebug__)) mzm_f32_sqrt(f32 x) {
  __m128 v = _mm_load_ss(&x);
  v = _mm_sqrt_ss(v);
  _mm_store_ss(&x, v);
  return x;
}

#define MZM_PI 3.141592654f
#define MZM_2PI 6.283185307f
#define MZM_1_DIV_PI 0.318309886f
#define MZM_1_DIV_2PI 0.159154943f
#define MZM_PI_DIV_2 1.570796327f
#define MZM_PI_DIV_4 0.785398163f

typedef struct mzm_vec3 {
  f32 x;
  f32 y;
  f32 z;
} mzm_vec3_t;

inline mzm_vec3_t mzm_vec3(f32 x, f32 y, f32 z) { return (mzm_vec3_t){x, y, z}; }

inline mzm_vec3_t mzm_vec3_from_scalar(f32 x) { return mzm_vec3(x, x, x); }

inline f32 mzm_vec3_dot(const mzm_vec3_t *a, const mzm_vec3_t *b) {
  return a->x * b->x + a->y * b->y + a->z * b->z;
}

inline f32 mzm_f32_abs(f32 x) {
  union {
    f32 f;
    u32 u;
  } fu;
  fu.f = x;
  fu.u &= 0x7fffffff;
  return fu.f;
}

inline i32 mzm_f32_equal_epsilon(f32 a, f32 b, f32 epsilon) {
  return mzm_f32_abs(a - b) <= epsilon;
}

inline u32 mzm_u32_rand(u32 *state) {
  MZ_ASSERT(state);
  *state = *state * 1103515245 + 12345;
  return (*state >> 16) & 0x7fff;
}

inline f32 mzm_f32_rand(u32 *state) {
  MZ_ASSERT(state);
  u32 result = (127 << 23) | (mzm_u32_rand(state) << 8);
  return *(f32 *)&result - 1.0f;
}

inline f32 mzm_f32_rand_range(u32 *state, f32 begin, f32 end) {
  MZ_ASSERT(state && begin < end);
  return begin + (end - begin) * mzm_f32_rand(state);
}

inline f32 _mzm_f32_sin_reduction(f32 x) {
  f32 quotient = MZM_1_DIV_2PI * x;
  quotient = x >= 0.0f ? (f32)((i32)(quotient + 0.5f)) : (f32)((i32)(quotient - 0.5f));
  f32 y = x - MZM_2PI * quotient;
  if (y > MZM_PI_DIV_2) {
    y = MZM_PI - y;
  } else if (y < -MZM_PI_DIV_2) {
    y = -MZM_PI - y;
  }
  return y;
}

inline f32 _mzm_f32_cos_reduction(f32 x, f32 *out_sign) {
  MZ_ASSERT(out_sign);
  f32 quotient = MZM_1_DIV_2PI * x;
  quotient = x >= 0.0f ? (f32)((i32)(quotient + 0.5f)) : (f32)((i32)(quotient - 0.5f));
  f32 y = x - MZM_2PI * quotient;
  if (y > MZM_PI_DIV_2) {
    y = MZM_PI - y;
    *out_sign = -1.0f;
  } else if (y < -MZM_PI_DIV_2) {
    y = -MZM_PI - y;
    *out_sign = -1.0f;
  } else {
    *out_sign = 1.0f;
  }
  return y;
}

inline f32 mzm_f32_sin(f32 x) {
  f32 y = _mzm_f32_sin_reduction(x);
  f32 yy = y * y;
  x = -2.3889859e-08f * yy + 2.7525562e-06f;
  x = x * yy - 0.00019840874f;
  x = x * yy + 0.0083333310f;
  x = x * yy - 0.16666667f;
  x = x * yy + 1.0f;
  return x * y;
}

inline f32 mzm_f32_sin_fast(f32 x) {
  f32 y = _mzm_f32_sin_reduction(x);
  f32 yy = y * y;
  x = -0.00018524670f * yy + 0.0083139502f;
  x = x * yy - 0.16665852f;
  x = x * yy + 1.0f;
  return x * y;
}

inline f32 mzm_f32_cos(f32 x) {
  f32 sign;
  f32 y = _mzm_f32_cos_reduction(x, &sign);
  f32 yy = y * y;
  x = -2.6051615e-07f * yy + 2.4760495e-05f;
  x = x * yy - 0.0013888378f;
  x = x * yy + 0.041666638f;
  x = x * yy - 0.5f;
  x = x * yy + 1.0f;
  return x * sign;
}

inline f32 mzm_f32_cos_fast(f32 x) {
  f32 sign;
  f32 y = _mzm_f32_cos_reduction(x, &sign);
  f32 yy = y * y;
  x = -0.0012712436f * yy + 0.041493919f;
  x = x * yy - 0.49992746f;
  x = x * yy + 1.0f;
  return sign * x;
}

void mzm_unit_tests(void);

#endif // #ifndef MZ_MATH_INCLUDED_

#ifdef MZ_MATH_IMPLEMENTATION

void mzm_unit_tests(void) {
  MZ_ASSERT(mzm_f32_equal_epsilon(mzm_f32_sqrt(2.0f), 1.414213562f, 0.00001f));
  MZ_ASSERT(mzm_f32_abs(-1.0f) == 1.0f);
  MZ_ASSERT(mzm_f32_abs(1.0f) == 1.0f);
  MZ_ASSERT(mzm_f32_equal_epsilon(mzm_f32_sin(123.123f), -0.56537f, 0.00001f));
  MZ_ASSERT(mzm_f32_equal_epsilon(mzm_f32_sin_fast(123.123f), -0.56537f, 0.00001f));
  MZ_ASSERT(mzm_f32_equal_epsilon(mzm_f32_cos(123.123f), -0.82483472f, 0.00001f));
  MZ_ASSERT(mzm_f32_equal_epsilon(mzm_f32_cos_fast(123.123f), -0.82483472f, 0.0001f));
}

#undef MZ_MATH_IMPLEMENTATION
#endif // #ifdef MZ_MATH_IMPLEMENTATION
