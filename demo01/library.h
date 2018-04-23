#pragma once
#include "types.h"

void *lib_load_file(const char *filename, u32 *ofilesize);
f64 lib_get_time(void);

void *mem_alloc(size_t size);
void *mem_realloc(void *addr, size_t size);
void mem_free(void *addr);
