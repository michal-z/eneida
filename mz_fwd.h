#pragma once

#ifndef _WIN32
#error _WIN32 not defined.
#endif

#ifndef _WIN64
#error _WIN64 not defined.
#endif

#ifndef __clang__
#error clang compiler is required.
#endif

#define NULL ((void *)0)

#ifdef _DEBUG
#define MZ_ASSERT(expression)                                                                      \
  if (!(expression))                                                                               \
  __debugbreak()
#else
#define MZ_ASSERT(expression) ((void)0)
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef float f32;
typedef double f64;
typedef long long ptrdiff_t;
typedef unsigned long long size_t;
