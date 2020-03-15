#ifndef MZ_LIBRARY_INCLUDED_
#define MZ_LIBRARY_INCLUDED_

#include "mz_fwd.h"

#define MZL_FREE(addr)                                                                             \
  if ((addr)) {                                                                                    \
    free((addr));                                                                                  \
    (addr) = NULL;                                                                                 \
  }

void *mzl_load_file(const char *filename, u32 *out_file_size);

f64 mzl_get_time(void);

void mzl_update_frame_stats(void *window, const char *name, f64 *out_time, f32 *out_delta_time);

void *mzl_create_window(const char *name, u32 width, u32 height);

void mzl_load_api(void);

#endif // #ifndef MZ_LIBRARY_INCLUDED_

#ifdef MZ_LIBRARY_IMPLEMENTATION

#include "mz_tinywin.h"

f64 mzl_get_time(void) {
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

void mzl_update_frame_stats(void *window, const char *name, f64 *out_time, f32 *out_delta_time) {
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
    wsprintf(header, "[%d fps  %d us] %s", (i32)fps, (i32)ms, name);
    SetWindowText(window, header);
    header_refresh_time = *out_time;
    num_frames = 0;
  }
  num_frames++;
}

static i64 __stdcall _mzl_process_window_message(void *window, u32 message, u64 wparam,
                                                 i64 lparam) {
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
  return DefWindowProc(window, message, wparam, lparam);
}

void *mzl_create_window(const char *name, u32 width, u32 height) {
  WNDCLASS winclass = {
      .lpfnWndProc = _mzl_process_window_message,
      .hInstance = GetModuleHandle(NULL),
      .hCursor = LoadCursor(NULL, IDC_ARROW),
      .lpszClassName = name,
  };
  if (!RegisterClass(&winclass)) {
    MZ_ASSERT(0);
  }

  RECT rect = {0, 0, (i32)width, (i32)height};
  AdjustWindowRect(&rect, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, 0);

  void *window = CreateWindowEx(
      0, name, name, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE,
      CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL,
      NULL, 0);
  MZ_ASSERT(window);
  return window;
}

void *mzl_load_file(const char *filename, u32 *out_file_size) {
  MZ_ASSERT(out_file_size);
  void *handle = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
  MZ_ASSERT(handle != (void *)-1);
  u32 size = GetFileSize(handle, NULL);
  char *content = malloc(size);
  u32 bytes_read;
  ReadFile(handle, content, size, &bytes_read, NULL);
  CloseHandle(handle);
  MZ_ASSERT(bytes_read == size);
  *out_file_size = size;
  return content;
}

void *malloc(u64 size) {
  MZ_ASSERT(size > 0);
  void *mem = HeapAlloc(GetProcessHeap(), 0, size);
  if (!mem) {
    OutputDebugString("malloc: Failed to allocate memory.\n");
    MZ_ASSERT(0);
    ExitProcess(1);
  }
  return mem;
}

void *realloc(void *addr, u64 size) {
  MZ_ASSERT(size > 0);
  if (addr == NULL) {
    return malloc(size);
  } else {
    void *mem = HeapReAlloc(GetProcessHeap(), 0, addr, size);
    if (!mem) {
      OutputDebugString("realloc: Failed to re-allocate memory.\n");
      MZ_ASSERT(0);
      ExitProcess(1);
    }
    return mem;
  }
}

void free(void *addr) {
  MZ_ASSERT(addr);
  if (!HeapFree(GetProcessHeap(), 0, addr)) {
    OutputDebugString("free: Failed to free memory.\n");
    MZ_ASSERT(0);
    ExitProcess(1);
  }
}

typedef void *(*_mzl_memset_t)(void *dest, i32 value, u64 num_bytes);
typedef void *(*_mzl_memcpy_t)(void *restrict dest, const void *restrict src, u64 num_bytes);
typedef void *(*_mzl_memmove_t)(void *dest, const void *src, u64 num_bytes);
typedef i32 (*_mzl_memcmp_t)(const void *ptr1, const void *ptr2, u64 num_bytes);
typedef i32 (*_mzl_strcmp_t)(const char *str1, const char *str2);
typedef u64 (*_mzl_strlen_t)(const char *str);

static _mzl_memset_t _mzl_memset;
static _mzl_memcpy_t _mzl_memcpy;
static _mzl_memmove_t _mzl_memmove;
static _mzl_memcmp_t _mzl_memcmp;
static _mzl_strcmp_t _mzl_strcmp;
static _mzl_strlen_t _mzl_strlen;

void mzl_load_api(void) {
  void *__stdcall LoadLibraryA(const char *dll_name);
  void *__stdcall GetProcAddress(void *dll, const char *proc);

  void *ucrtbase_dll = LoadLibraryA("ucrtbase.dll");
  _mzl_memset = (_mzl_memset_t)GetProcAddress(ucrtbase_dll, "memset");
  _mzl_memcpy = (_mzl_memcpy_t)GetProcAddress(ucrtbase_dll, "memcpy");
  _mzl_memmove = (_mzl_memmove_t)GetProcAddress(ucrtbase_dll, "memmove");
  _mzl_memcmp = (_mzl_memcmp_t)GetProcAddress(ucrtbase_dll, "memcmp");
  _mzl_strcmp = (_mzl_strcmp_t)GetProcAddress(ucrtbase_dll, "strcmp");
  _mzl_strlen = (_mzl_strlen_t)GetProcAddress(ucrtbase_dll, "strlen");
}

void *memset(void *dest, i32 value, u64 num_bytes) { return _mzl_memset(dest, value, num_bytes); }

void *memcpy(void *restrict dest, const void *restrict src, u64 num_bytes) {
  return _mzl_memcpy(dest, src, num_bytes);
}

void *memmove(void *dest, const void *src, u64 num_bytes) {
  return _mzl_memmove(dest, src, num_bytes);
}

i32 memcmp(const void *ptr1, const void *ptr2, u64 num_bytes) {
  return _mzl_memcmp(ptr1, ptr2, num_bytes);
}

i32 strcmp(const char *str1, const char *str2) { return _mzl_strcmp(str1, str2); }

u64 strlen(const char *str) { return _mzl_strlen(str); }

#undef MZ_LIBRARY_IMPLEMENTATION
#endif // #ifdef MZ_LIBRARY_IMPLEMENTATION
