#ifndef MZ_LIBRARY_INCLUDED__
#define MZ_LIBRARY_INCLUDED__

#include "mz_types.h"

#define MZL_MALLOC(size) mzl_malloc((size), __FILE__, __LINE__)
#define MZL_REALLOC(addr, size) mzl_realloc((addr), (size), __FILE__, __LINE__)
#define MZL_FREE(addr)                        \
    if ((addr)) {                             \
        mzl_free((addr), __FILE__, __LINE__); \
        (addr) = NULL;                        \
    }

void mzl_load_file(const char *filename, u32 *out_size, u8 **out_content);
f64 mzl_get_time(void);
void mzl_update_frame_stats(void *window, const char *name, f64 *out_time, f32 *out_delta_time);
void *mzl_create_window(const char *name, u32 width, u32 height);
void *mzl_malloc(u64 size, const char *filename, i32 line);
void *mzl_realloc(void *addr, u64 size, const char *filename, i32 line);
void mzl_free(void *addr, const char *filename, i32 line);

#endif // #ifndef MZ_LIBRARY_INCLUDED__

//
//
// Stop now, if you are only interested in the API.
// Below, you find the implementation.
//
//

#ifdef __INTELLISENSE__
// This makes MSVC intellisense work.
#define MZ_LIBRARY_IMPLEMENTATION
#endif

#ifdef MZ_LIBRARY_IMPLEMENTATION

#include "mz_libc.h"
#include "mz_windows.h"

void mzl_load_file(const char *filename, u32 *out_size, u8 **out_content)
{
    assert(out_size && out_content);
    *out_size = 0;
    *out_content = NULL;
    FILE *file = fopen(filename, "rb");
    if (!file) {
        assert(0);
        return;
    }
    fseek(file, 0, SEEK_END);
    i32 size = ftell(file);
    if (size <= 0) {
        assert(0);
        return;
    }
    u8 *content = MZL_MALLOC(size);
    fseek(file, 0, SEEK_SET);
    fread(content, 1, size, file);
    fclose(file);
    *out_size = size;
    *out_content = content;
}

f64 mzl_get_time(void)
{
    static i64 start_counter;
    static i64 frequency;
    if (start_counter == 0) {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start_counter);
    }
    i64 counter;
    QueryPerformanceCounter(&counter);
    return (counter - start_counter) / (f64)frequency;
}

void mzl_update_frame_stats(void *window, const char *name, f64 *out_time, f32 *out_delta_time)
{
    static f64 previous_time = -1.0;
    static f64 header_refresh_time = 0.0;
    static u32 num_frames = 0;

    if (previous_time < 0.0) {
        previous_time = mzl_get_time();
        header_refresh_time = previous_time;
    }

    *out_time = mzl_get_time();
    *out_delta_time = (f32)(*out_time - previous_time);
    previous_time = *out_time;

    if ((*out_time - header_refresh_time) >= 1.0) {
        f64 fps = num_frames / (*out_time - header_refresh_time);
        f64 ms = (1.0 / fps) * 1000000.0;
        char header[256];
        wsprintfA(header, "[%d fps  %d us] %s", (i32)fps, (i32)ms, name);
        SetWindowTextA(window, header);
        header_refresh_time = *out_time;
        num_frames = 0;
    }
    num_frames++;
}

static i64 __stdcall _mzl_process_window_message(void *window, u32 message, u64 wparam, i64 lparam)
{
    switch (message) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (wparam == VK_ESCAPE) {
            PostQuitMessage(0);
            return 0;
        }
        break;
    }
    return DefWindowProcA(window, message, wparam, lparam);
}

void *mzl_create_window(const char *name, u32 width, u32 height)
{
    WNDCLASS winclass = {
        .lpfnWndProc = _mzl_process_window_message,
        .hInstance = GetModuleHandleA(NULL),
        .hCursor = LoadCursorA(NULL, IDC_ARROW),
        .lpszClassName = name,
    };
    if (!RegisterClassA(&winclass)) {
        assert(0);
    }

    RECT rect = { 0, 0, (i32)width, (i32)height };
    AdjustWindowRect(&rect, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, 0);

    void *window = CreateWindowExA(0, name, name, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, NULL, 0);
    assert(window);
    return window;
}

void *mzl_malloc(u64 size, const char *filename, i32 line)
{
    (void)filename;
    (void)line;
    return malloc(size);
}

void *mzl_realloc(void *addr, u64 size, const char *filename, i32 line)
{
    (void)filename;
    (void)line;
    return realloc(addr, size);
}

void mzl_free(void *addr, const char *filename, i32 line)
{
    (void)filename;
    (void)line;
    free(addr);
}

#endif // #ifdef MZ_LIBRARY_IMPLEMENTATION
