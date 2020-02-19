#ifndef MZ_TYPES_INCLUDED__
#define MZ_TYPES_INCLUDED__

#define NULL ((void *)0)

#ifdef _DEBUG
#define assert(expression) \
    if (!(expression)) {   \
        __debugbreak();    \
    }
#else
#define assert(expression)
#endif

#define restrict __restrict

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
typedef i64 ptrdiff_t;
typedef u64 size_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
typedef long long int64_t;

#endif // #ifndef MZ_TYPES_INCLUDED__
