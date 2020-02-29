#include "mz_library.h"
#define memmove mzl_memmove
#define memset mzl_memset
#define memcpy mzl_memcpy
#define memcmp mzl_memcmp
#define strcmp mzl_strcmp
#define strlen mzl_strlen
#define STBDS_REALLOC(c, p, s) mzl_realloc(p, s, __FILE__, __LINE__)
#define STBDS_FREE(c, p) mzl_free(p, __FILE__, __LINE__)
typedef unsigned long long size_t;
typedef long long ptrdiff_t;
#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"
