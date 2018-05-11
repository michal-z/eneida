#pragma once
#include "types.h"

typedef struct {
    u32 size;
    u32 capacity;
} array_header_t;

#define array_header(a) \
    ((array_header_t *)((char *)(a) - sizeof(array_header_t)))

#define array_size(a) ((a) ? array_header(a)->size : 0)
#define array_capacity(a) ((a) ? array_header(a)->capacity : 0)
#define array_full(a) (array_size(a) == array_capacity(a) ? 1 : 0)
#define array_free(a) ((a) ? (mem_free(array_header(a)), 0) : 0)

#define array_add(a, item) \
    (array_full(a) ? a = array_grow((a), sizeof(*(a))) : 0), \
    ((a)[array_header(a)->size++] = item)

void *mem_alloc(size_t size);
void *mem_realloc(void *addr, size_t size);
void mem_free(void *addr);

inline void *array_grow(void *array, u32 item_size)
{
    u32 size = array_size(array);
    u32 capacity = size ? size * 2 : 1;
    array = array ? array_header(array) : 0;
    array_header_t *ha = (array_header_t *)mem_realloc(array, capacity * item_size + sizeof(array_header_t));
    ha->size = size;
    ha->capacity = capacity;
    return (void *)(ha + 1);
}

void *lib_load_file(const char *filename, u32 *ofilesize);
f64 lib_get_time(void);
