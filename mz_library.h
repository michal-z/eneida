#ifndef MZ_LIBRARY_INCLUDED_
#define MZ_LIBRARY_INCLUDED_

#include "mz_fwd.h"

#define MZL_MALLOC(size) mzl_malloc((size), __FILE__, __LINE__)
#define MZL_REALLOC(addr, size) mzl_realloc((addr), (size), __FILE__, __LINE__)
#define MZL_FREE(addr)                                                         \
  if ((addr)) {                                                                \
    mzl_free((addr), __FILE__, __LINE__);                                      \
    (addr) = NULL;                                                             \
  }

void *mzl_load_file(const char *filename, u32 *out_file_size);

f64 mzl_get_time(void);

void mzl_update_frame_stats(void *window, const char *name, f64 *out_time,
                            f32 *out_delta_time);

void *mzl_create_window(const char *name, u32 width, u32 height);

void *mzl_malloc(u64 size, const char *filename, i32 line);

void *mzl_realloc(void *addr, u64 size, const char *filename, i32 line);

void mzl_free(void *addr, const char *filename, i32 line);

void *mzl_memset(void *ptr, int value, u64 num_bytes);

void *mzl_memcpy(void *destination, const void *source, u64 num_bytes);

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

void mzl_update_frame_stats(void *window, const char *name, f64 *out_time,
                            f32 *out_delta_time) {
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

static i64 __stdcall _mzl_process_window_message(void *window, u32 message,
                                                 u64 wparam, i64 lparam) {
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
    assert(0);
  }

  RECT rect = {0, 0, (i32)width, (i32)height};
  AdjustWindowRect(&rect,
                   WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, 0);

  void *window = CreateWindowEx(
      0, name, name,
      WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE,
      CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left,
      rect.bottom - rect.top, NULL, NULL, NULL, 0);
  assert(window);
  return window;
}

void *mzl_load_file(const char *filename, u32 *out_file_size) {
  assert(out_file_size);
  void *handle =
      CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
  assert(handle != (void *)-1);
  u32 size = GetFileSize(handle, NULL);
  char *content = (char *)MZL_MALLOC(size);
  u32 bytes_read;
  ReadFile(handle, content, size, &bytes_read, NULL);
  CloseHandle(handle);
  assert(bytes_read == size);
  *out_file_size = size;
  return content;
}

void *mzl_malloc(u64 size, const char *filename, i32 line) {
  assert(size > 0);
  void *mem = HeapAlloc(GetProcessHeap(), 0, size);
  if (!mem) {
    char info[256];
    wsprintf(info, "mzl_malloc: Failed to allocate memory (%s:%d).\n", filename,
             line);
    OutputDebugString(info);
    assert(0);
    ExitProcess(1);
  }
  return mem;
}

void *mzl_realloc(void *addr, u64 size, const char *filename, i32 line) {
  assert(size > 0);
  if (addr == NULL) {
    return mzl_malloc(size, filename, line);
  } else {
    void *mem = HeapReAlloc(GetProcessHeap(), 0, addr, size);
    if (!mem) {
      char info[256];
      wsprintf(info, "mzl_realloc: Failed to re-allocate memory (%s:%d).\n",
               filename, line);
      OutputDebugString(info);
      assert(0);
      ExitProcess(1);
    }
    return mem;
  }
}

void mzl_free(void *addr, const char *filename, i32 line) {
  assert(addr);
  if (!HeapFree(GetProcessHeap(), 0, addr)) {
    char info[256];
    wsprintf(info, "mzl_free: Failed to free memory (%s:%d).\n", filename,
             line);
    OutputDebugString(info);
    assert(0);
    ExitProcess(1);
  }
}

void *mzl_memset(void *ptr, int value, u64 num_bytes) {
  assert(ptr && value < 256 && num_bytes > 0);
  // Call our (slow) assembly memset.
  void *memset(void *ptr, int value, u64 num_bytes);
  return memset(ptr, value, num_bytes);
}

void *mzl_memcpy(void *destination, const void *source, u64 num_bytes) {
  assert(destination && source && num_bytes > 0);
  // Call our (slow) assembly memcpy.
  void *memcpy(void *destination, const void *source, u64 num_bytes);
  return memcpy(destination, source, num_bytes);
}

#undef MZ_LIBRARY_IMPLEMENTATION
#endif // #ifdef MZ_LIBRARY_IMPLEMENTATION
