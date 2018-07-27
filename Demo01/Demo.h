#pragma once

typedef struct array_header
{
    u32 Size;
    u32 Capacity;
} array_header;

#define ArrayHeader(A) \
    ((array_header *)((char *)(A) - sizeof(array_header)))

#define ArraySize(A) ((A) ? ArrayHeader(A)->Size : 0)
#define ArrayCapacity(A) ((A) ? ArrayHeader(A)->Capacity : 0)
#define ArrayIsFull(A) (ArraySize(A) == ArrayCapacity(A) ? 1 : 0)
#define ArrayFree(A) ((A) ? (MemFree(ArrayHeader(A)), 0) : 0)

#define ArrayAdd(A, Item) \
    (ArrayIsFull(A) ? A = ArrayGrow((A), sizeof(*(A))) : 0), \
    ((A)[ArrayHeader(A)->Size++] = (Item))

inline void *
ArrayGrow(void *Array, u32 ItemSize)
{
    u32 Size = ArraySize(Array);
    u32 Capacity = Size ? Size * 2 : 1;
    Array = Array ? ArrayHeader(Array) : NULL;
    array_header *Ha = (array_header *)MemReAllocate(Array, Capacity * ItemSize + sizeof(array_header));
    Ha->Size = Size;
    Ha->Capacity = Capacity;
    return (void *)(Ha + 1);
}
