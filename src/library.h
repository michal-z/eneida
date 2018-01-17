#pragma once

#include "types.h"

char *load_file(const char *filename);

void *memset(void *dest, i32 value, size_t count);
#pragma intrinsic(memset)

void *malloc(size_t size);
void *realloc(void *addr, size_t size);
void free(void *addr);
