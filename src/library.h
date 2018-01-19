#pragma once
#include "types.h"

void *load_file(const char *filename, u32 *ofilesize);

// In debug mode compiler generates 'call sqrtf' (implementation is in asmlib.asm).
// In release mode compiler generates 'sqrtss xmmA, xmmB' instruction.
float sqrtf(float x);

void *malloc(size_t size);
void *realloc(void *addr, size_t size);
void free(void *addr);
