#pragma once
#include "types.h"

void *load_file(const char *filename, u32 *ofilesize);

// In debug mode compiler generates 'call sqrtf' (implementation is in asmlib.asm).
// In release mode compiler generates 'sqrtss xmmA, xmmB' instruction.
float sqrtf(float x);

void *malloc(size_t size);
void *realloc(void *addr, size_t size);
void free(void *addr);

void d3d_flush(void);
ID3D12Resource *d3d_create_buffer(ID3D12Device *d3d, D3D12_HEAP_TYPE heap_type, u64 size);
