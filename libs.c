#include "mz_library.h"
typedef unsigned long long size_t;
typedef long long ptrdiff_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef char int8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
#define memset mzl_memset
#define memcpy mzl_memcpy
#define memmove mzl_memmove
#define memcmp mzl_memcmp
#define strcmp mzl_strcmp
#define strlen mzl_strlen
#define strncmp mzl_strncmp
#define strchr mzl_strchr
#define strstr mzl_strstr
#define strncpy mzl_strncpy
#define strrchr mzl_strrchr
#define strcpy mzl_strcpy
#define INT_MAX 2147483647
#define SIZE_MAX 0xffffffffffffffffull

#define STBI_MALLOC(sz) mzl_malloc(sz, __FILE__, __LINE__)
#define STBI_REALLOC(p, newsz) mzl_realloc(p, newsz, __FILE__, __LINE__)
#define STBI_FREE(p) mzl_free(p, __FILE__, __LINE__)
#define STB_IMAGE_IMPLEMENTATION
#include "mz_stb_image.h"

#define STBDS_REALLOC(c, p, s) mzl_realloc(p, s, __FILE__, __LINE__)
#define STBDS_FREE(c, p) mzl_free(p, __FILE__, __LINE__)
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
#define CGLTF_MALLOC(size) mzl_malloc(size, __FILE__, __LINE__)
#define CGLTF_FREE(addr) mzl_free(addr, __FILE__, __LINE__)
#define CGLTF_ATOF mzl_atof
#define CGLTF_ATOI mzl_atoi
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"
#pragma clang diagnostic pop
