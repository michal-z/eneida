#pragma once

#ifndef _WIN32
#error _WIN32 not defined.
#endif

#ifndef _WIN64
#error _WIN64 not defined.
#endif

#define NULL ((void *)0)

#ifdef _DEBUG
#define assert(expression)                                                                         \
  if (!(expression))                                                                               \
  __debugbreak()
#else
#define assert(expression)
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
