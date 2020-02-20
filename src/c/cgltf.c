#define CGLTF_IMPLEMENTATION
#define CGLTF_MALLOC(size) mzl_malloc(size, __FILE__, __LINE__)
#define CGLTF_FREE(addr) mzl_free((addr), __FILE__, __LINE__)
#include "cgltf.h"
