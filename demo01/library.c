#include "library.h"
#include "windows.h"


void *lib_load_file(const char *filename, u32 *ofilesize)
{
    assert(ofilesize);
    void *handle = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    assert(handle != (void *)-1);
    u32 size = GetFileSize(handle, NULL);
    char *content = (char *)mem_alloc(size);
    u32 bytes_read;
    ReadFile(handle, content, size, &bytes_read, NULL);
    CloseHandle(handle);
    assert(bytes_read == size);
    *ofilesize = size;
    return content;
}

f64 lib_get_time(void)
{
    static i64 frequency, start_counter;
    if (frequency == 0) {
        QueryPerformanceCounter(&start_counter);
        QueryPerformanceFrequency(&frequency);
    }
    i64 counter;
    QueryPerformanceCounter(&counter);
    return (counter - start_counter) / (f64)frequency;
}

void *mem_alloc(size_t size)
{
    assert(size > 0);
    void *mem = HeapAlloc(GetProcessHeap(), 0, size);
    if (!mem) {
        OutputDebugString("Failed to allocate memory!");
        assert(0);
        ExitProcess(1);
    }
    return mem;
}

void *mem_realloc(void *addr, size_t size)
{
    assert(size > 0);
    if (addr == NULL)
        return mem_alloc(size);
    else {
        void *mem = HeapReAlloc(GetProcessHeap(), 0, addr, size);
        if (!mem) {
            OutputDebugString("Failed to reallocate memory!");
            assert(0);
            ExitProcess(1);
        }
        return mem;
    }
}

void mem_free(void *addr)
{
    assert(addr);
    if (!HeapFree(GetProcessHeap(), 0, addr)) {
        OutputDebugString("Failed to free memory!");
        assert(0);
        ExitProcess(1);
    }
}
