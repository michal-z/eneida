#pragma once

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;
typedef char i8;
typedef short i16;
typedef long i32;
typedef long long i64;

#define NULL ((void *)0)

#ifdef _DEBUG
#define assert(expression) if (!(expression)) { __debugbreak(); }
#else
#define assert(expression)
#endif

char *load_text_file(const char *filename);

void *memset(void *dest, int value, size_t count);
#pragma intrinsic(memset)

void *malloc(size_t size);
void free(void *addr);

#include "math.h"
#include "opengl.h"
