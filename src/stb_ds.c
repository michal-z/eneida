#define STB_DS_IMPLEMENTATION
#define STBDS_REALLOC(c, p, s) mzl_realloc(p, s, __FILE__, __LINE__)
#define STBDS_FREE(c, p) mzl_free(p, __FILE__, __LINE__)
#include "stb_ds.h"
