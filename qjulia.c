#define NULL ((void *)0)

#ifdef _DEBUG
#define assert(expression) if (!(expression)) { __debugbreak(); }
#else
#define assert(expression)
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;

#define PM_REMOVE 0x0001
#define WM_QUIT 0x0012
#define WM_DESTROY 0x0002
#define WM_KEYDOWN 0x0100
#define VK_ESCAPE 0x001b
#define VK_CONTROL 0x11
#define WS_OVERLAPPED 0x00000000L
#define WS_VISIBLE 0x10000000L
#define WS_CAPTION 0x00C00000L
#define WS_SYSMENU 0x00080000L
#define WS_MINIMIZEBOX 0x00020000L
#define CW_USEDEFAULT ((u32)0x80000000)
#define MAKEINTRESOURCE(i) ((char *)((u64)((u16)(i))))
#define IDC_ARROW MAKEINTRESOURCE(32512)
#define GENERIC_READ 0x80000000
#define OPEN_EXISTING 3
#define INFINITE 0xFFFFFFFF
#define STANDARD_RIGHTS_REQUIRED 0x000F0000L
#define SYNCHRONIZE 0x00100000L
#define EVENT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3)
#define MEM_COMMIT 0x1000
#define MEM_RESERVE 0x2000
#define MEM_RELEASE 0x8000
#define PAGE_READWRITE 0x04

typedef struct SECURITY_ATTRIBUTES {
    u32 nLength;
    void *lpSecurityDescriptor;
    i32 bInheritHandle;
} SECURITY_ATTRIBUTES;

typedef struct RECT {
    i32 left;
    i32 top;
    i32 right;
    i32 bottom;
} RECT;

typedef struct POINT {
    i32 x;
    i32 y;
} POINT;

typedef struct MSG {
    void *hwnd;
    u32 message;
    u64 wParam;
    i64 lParam;
    u32 time;
    POINT pt;
} MSG;

typedef struct WNDCLASS {
    u32 style;
    void *lpfnWndProc;
    i32 cbClsExtra;
    i32 cbWndExtra;
    void *hInstance;
    void *hIcon;
    void *hCursor;
    void *hbrBackground;
    const char *lpszMenuName;
    const char *lpszClassName;
} WNDCLASS;

extern void *__stdcall LoadLibraryA(const char *);
extern void *__stdcall GetProcAddress(void *, const char *);

i32 _fltused;

static void (__stdcall *OutputDebugString)(const char *);
static i32 (__stdcall *QueryPerformanceCounter)(i64 *);
static i32 (__stdcall *QueryPerformanceFrequency)(i64 *);
static void *(__stdcall *VirtualAlloc)(void *, u64, u32, u32);
static i32 (__stdcall *VirtualFree)(void *, u64, u32);
static void (__stdcall *ExitProcess)(u32);
static void *(__stdcall *CreateFile)(const char *, u32, u32, SECURITY_ATTRIBUTES *, u32, u32, void *);
static i32 (__stdcall *ReadFile)(void *, void *, u32, u32 *, void *);
static u32 (__stdcall *GetFileSize)(void *, u32 *);
static i32 (__stdcall *CloseHandle)(void *);
static void *(__stdcall *GetModuleHandle)(const char *);
static void (__stdcall *Sleep)(u32);
static void *(__stdcall *HeapAlloc)(void *, u32, u64);
static i32 (__stdcall *HeapFree)(void *, u32, void *);
static void *(__stdcall *HeapReAlloc)(void *, u32, void *, u64);
static void *(__stdcall *GetProcessHeap)(void);
static void *(__stdcall *CreateEventEx)(SECURITY_ATTRIBUTES *, const char *, u32, u32);
static u32 (__stdcall *WaitForSingleObject)(void *, u32);

static i32 (__stdcall *PeekMessage)(MSG *, void *, u32, u32, u32);
static i64 (__stdcall *DispatchMessage)(const MSG *);
static void (__stdcall *PostQuitMessage)(i32);
static i64 (__stdcall *DefWindowProc)(void *, u32, u64, i64);
static void *(__stdcall *LoadCursor)(void *, const char *);
static i16 (__stdcall *RegisterClass)(const WNDCLASS *);
static void *(__stdcall *CreateWindowEx)(u32, const char *, const char *, u32, i32, i32, i32, i32, void *, void *, void *, void *);
static i32 (__stdcall *AdjustWindowRect)(RECT *, u32, i32);
static i32 (__cdecl *wsprintf)(char *, const char *, ...);
static i32 (__stdcall *SetWindowText)(void *, const char *);
static i32 (__stdcall *SetProcessDPIAware)(void);
static void *(__stdcall *GetDC)(void *);
static i32 (__stdcall *MessageBox)(void *, const char *, const char *, u32);

#define k_demo_name "qjulia"
#define k_win_width 1280
#define k_win_height 720

static void *s_main_heap;

void *memset(void *dest, i32 value, u64 count);
#pragma intrinsic(memset)

#pragma function(memset)
void *memset(void *dest, i32 value, u64 count)
{
    __stosb((u8 *)dest, (u8)value, count);
    return dest;
}

static void *malloc(u64 size)
{
    assert(size > 0 && s_main_heap);

    void *mem = HeapAlloc(s_main_heap, 0, size);
    if (!mem) {
        OutputDebugString("Failed to allocate memory!");
        assert(0);
        ExitProcess(1);
    }
    return mem;
}

static void *realloc(void *addr, u64 size)
{
    assert(size > 0 && s_main_heap);

    if (addr == NULL)
        return malloc(size);
    else {
        void *mem = HeapReAlloc(s_main_heap, 0, addr, size);
        if (!mem) {
            OutputDebugString("Failed to reallocate memory!");
            assert(0);
            ExitProcess(1);
        }
        return mem;
    }
}

static void free(void *addr)
{
    assert(addr && s_main_heap);

    if (!HeapFree(s_main_heap, 0, addr)) {
        OutputDebugString("Failed to free memory!");
        assert(0);
        ExitProcess(1);
    }
}

static void *load_file(const char *filename, u32 *ofilesize)
{
    assert(filename && ofilesize);

    void *handle = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    assert(handle != (void *)-1);
    u32 size = GetFileSize(handle, NULL);
    u8 *content = (u8 *)malloc(size);
    u32 bytes_read;
    ReadFile(handle, content, size, &bytes_read, NULL);
    CloseHandle(handle);
    assert(bytes_read == size);
    *ofilesize = size;
    return content;
}

static double get_time(void)
{
    static i64 frequency, start_counter;
    if (frequency == 0) {
        QueryPerformanceCounter(&start_counter);
        QueryPerformanceFrequency(&frequency);
    }

    i64 counter;
    QueryPerformanceCounter(&counter);
    return (counter - start_counter) / (double)frequency;
}

static void update_frame_time(void *window, const char *name, double *time, float *delta_time)
{
    static double last_time;
    static double last_fps_time;
    static u32 frame_count;

    if (last_time == 0.0) {
        last_time = get_time();
        last_fps_time = last_time;
    }

    *time = get_time();
    *delta_time = (float)(*time - last_time);
    last_time = *time;

    if ((*time - last_fps_time) >= 1.0) {
        double fps = frame_count / (*time - last_fps_time);
        double us = (1.0 / fps) * 1000000.0;
        char text[256];
        wsprintf(text, "[%d fps  %d us] %s", (i32)fps, (i32)us, name);
        SetWindowText(window, text);
        last_fps_time = *time;
        frame_count = 0;
    }
    frame_count++;
}

static i64 __stdcall process_window_message(void *window, u32 message, u64 wparam, i64 lparam)
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
    return DefWindowProc(window, message, wparam, lparam);
}

static void *init_window(const char *name, u32 width, u32 height)
{
    WNDCLASS winclass = {
        .lpfnWndProc = process_window_message,
        .hInstance = GetModuleHandle(NULL),
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .lpszClassName = name,
    };
    if (!RegisterClass(&winclass))
        assert(0);

    RECT rect = { 0, 0, (i32)width, (i32)height };
    if (!AdjustWindowRect(&rect, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, 0))
        assert(0);

    void *window = CreateWindowEx(0, name, name, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE,
                                  CW_USEDEFAULT, CW_USEDEFAULT,
                                  rect.right - rect.left, rect.bottom - rect.top,
                                  NULL, NULL, NULL, 0);
    assert(window);
    return window;
}

void start(void)
{
    void *kernel32_dll = LoadLibraryA("kernel32.dll");
    OutputDebugString = GetProcAddress(kernel32_dll, "OutputDebugStringA");
    GetModuleHandle = GetProcAddress(kernel32_dll, "GetModuleHandleA");
    QueryPerformanceCounter = GetProcAddress(kernel32_dll, "QueryPerformanceCounter");
    QueryPerformanceFrequency = GetProcAddress(kernel32_dll, "QueryPerformanceFrequency");
    VirtualAlloc = GetProcAddress(kernel32_dll, "VirtualAlloc");
    VirtualFree = GetProcAddress(kernel32_dll, "VirtualFree");
    ExitProcess = GetProcAddress(kernel32_dll, "ExitProcess");
    CreateFile = GetProcAddress(kernel32_dll, "CreateFileA");
    ReadFile = GetProcAddress(kernel32_dll, "ReadFile");
    GetFileSize = GetProcAddress(kernel32_dll, "GetFileSize");
    CloseHandle = GetProcAddress(kernel32_dll, "CloseHandle");
    Sleep = GetProcAddress(kernel32_dll, "Sleep");
    HeapAlloc = GetProcAddress(kernel32_dll, "HeapAlloc");
    HeapFree = GetProcAddress(kernel32_dll, "HeapFree");
    HeapReAlloc = GetProcAddress(kernel32_dll, "HeapReAlloc");
    GetProcessHeap = GetProcAddress(kernel32_dll, "GetProcessHeap");
    CreateEventEx = GetProcAddress(kernel32_dll, "CreateEventExA");
    WaitForSingleObject = GetProcAddress(kernel32_dll, "WaitForSingleObject");

    void *user32_dll = LoadLibraryA("user32.dll");
    PeekMessage = GetProcAddress(user32_dll, "PeekMessageA");
    DispatchMessage = GetProcAddress(user32_dll, "DispatchMessageA");
    PostQuitMessage = GetProcAddress(user32_dll, "PostQuitMessage");
    DefWindowProc = GetProcAddress(user32_dll, "DefWindowProcA");
    LoadCursor = GetProcAddress(user32_dll, "LoadCursorA");
    RegisterClass = GetProcAddress(user32_dll, "RegisterClassA");
    CreateWindowEx = GetProcAddress(user32_dll, "CreateWindowExA");
    AdjustWindowRect = GetProcAddress(user32_dll, "AdjustWindowRect");
    wsprintf = GetProcAddress(user32_dll, "wsprintfA");
    SetWindowText = GetProcAddress(user32_dll, "SetWindowTextA");
    SetProcessDPIAware = GetProcAddress(user32_dll, "SetProcessDPIAware");
    GetDC = GetProcAddress(user32_dll, "GetDC");
    MessageBox = GetProcAddress(user32_dll, "MessageBoxA");

    SetProcessDPIAware();
    s_main_heap = GetProcessHeap();

    // init window
    void *window = init_window(k_demo_name, k_win_width, k_win_height);

    for (;;) {
        MSG message = { 0 };
        if (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
            DispatchMessage(&message);
            if (message.message == WM_QUIT)
                break;
        } else {
            double frame_time;
            float frame_delta_time;
            update_frame_time(window, k_demo_name, &frame_time, &frame_delta_time);
        }
    }

    ExitProcess(0);
}
