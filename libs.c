#include "mz_library.h"
typedef unsigned long long size_t;
typedef long long ptrdiff_t;
#define memset mzl_memset
#define memcpy mzl_memcpy
#define memmove mzl_memmove
#define memcmp mzl_memcmp
#define strcmp mzl_strcmp
#define strlen mzl_strlen
#define INT_MAX 2147483647

#define STBI_MALLOC(sz) mzl_malloc(sz, __FILE__, __LINE__)
#define STBI_REALLOC(p, newsz) mzl_realloc(p, newsz, __FILE__, __LINE__)
#define STBI_FREE(p) mzl_free(p, __FILE__, __LINE__)
#define STB_IMAGE_IMPLEMENTATION
#include "mz_stb_image.h"

#define STBDS_REALLOC(c, p, s) mzl_realloc(p, s, __FILE__, __LINE__)
#define STBDS_FREE(c, p) mzl_free(p, __FILE__, __LINE__)
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
