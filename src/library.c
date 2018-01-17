#include "library.h"
#define STB_PERLIN_IMPLEMENTATION
#include "stb_perlin.h"


#pragma function(memset)
void *memset(void *dest, i32 value, size_t count)
{
    __stosb((unsigned char *)dest, (unsigned char)value, count);
    return dest;
}
