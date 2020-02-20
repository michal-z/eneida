#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_SIMD
#define STBI_MALLOC(sz) mzl_malloc(sz, __FILE__, __LINE__)
#define STBI_REALLOC(p, newsz) mzl_realloc(p, newsz, __FILE__, __LINE__)
#define STBI_FREE(p) mzl_free(p, __FILE__, __LINE__)
#include "stb_image.h"
