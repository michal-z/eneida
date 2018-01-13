#include "demo01.h"
#include "opengl.h"


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
#define PFD_TYPE_RGBA 0
#define PFD_DOUBLEBUFFER 0x00000001
#define PFD_DRAW_TO_WINDOW 0x00000004
#define PFD_SUPPORT_OPENGL 0x00000020
#define GENERIC_READ 0x80000000
#define OPEN_EXISTING 3

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

typedef struct RECT {
    i32 left;
    i32 top;
    i32 right;
    i32 bottom;
} RECT;

typedef struct SECURITY_ATTRIBUTES {
    u32 nLength;
    void *lpSecurityDescriptor;
    i32 bInheritHandle;
} SECURITY_ATTRIBUTES;

typedef struct PIXELFORMATDESCRIPTOR {
    u16 nSize;
    u16 nVersion;
    u32 dwFlags;
    u8 iPixelType;
    u8 cColorBits;
    u8 cRedBits;
    u8 cRedShift;
    u8 cGreenBits;
    u8 cGreenShift;
    u8 cBlueBits;
    u8 cBlueShift;
    u8 cAlphaBits;
    u8 cAlphaShift;
    u8 cAccumBits;
    u8 cAccumRedBits;
    u8 cAccumGreenBits;
    u8 cAccumBlueBits;
    u8 cAccumAlphaBits;
    u8 cDepthBits;
    u8 cStencilBits;
    u8 cAuxBuffers;
    u8 iLayerType;
    u8 bReserved;
    u32 dwLayerMask;
    u32 dwVisibleMask;
    u32 dwDamageMask;
} PIXELFORMATDESCRIPTOR;

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
static void * (__stdcall *GetProcessHeap)(void);

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

static i32 (__stdcall *SwapBuffers)(void *);
static i32 (__stdcall *SetPixelFormat)(void *, i32, const PIXELFORMATDESCRIPTOR *);
static i32 (__stdcall *ChoosePixelFormat)(void *, const PIXELFORMATDESCRIPTOR *);

PFNGLCREATEVERTEXARRAYSPROC glCreateVertexArrays;
PFNGLCREATEPROGRAMPIPELINESPROC glCreateProgramPipelines;
PFNGLCREATESHADERPROGRAMVPROC glCreateShaderProgramv;
PFNGLCREATETEXTURESPROC glCreateTextures;
PFNGLDELETEVERTEXARRAYSPROC glDeleteVertexArrays;
PFNGLDELETEPROGRAMPIPELINESPROC glDeleteProgramPipelines;
PFNGLDELETEPROGRAMPROC glDeleteProgram;
PFNGLDELETETEXTURESPROC glDeleteTextures;
PFNGLUSEPROGRAMSTAGESPROC glUseProgramStages;
PFNGLBINDPROGRAMPIPELINEPROC glBindProgramPipeline;
PFNGLBINDVERTEXARRAYPROC glBindVertexArray;
PFNGLDRAWARRAYSPROC glDrawArrays;
PFNGLDISPATCHCOMPUTEPROC glDispatchCompute;
PFNGLTEXTURESTORAGE2DPROC glTextureStorage2D;
PFNGLTEXTUREPARAMETERIPROC glTextureParameteri;
PFNGLBINDTEXTUREUNITPROC glBindTextureUnit;
PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
PFNGLCREATEBUFFERSPROC glCreateBuffers;
PFNGLDELETEBUFFERSPROC glDeleteBuffers;
PFNGLNAMEDBUFFERSTORAGEPROC glNamedBufferStorage;
PFNGLVERTEXARRAYATTRIBBINDINGPROC glVertexArrayAttribBinding;
PFNGLVERTEXARRAYATTRIBFORMATPROC glVertexArrayAttribFormat;
PFNGLVERTEXARRAYVERTEXBUFFERPROC glVertexArrayVertexBuffer;
PFNGLENABLEVERTEXARRAYATTRIBPROC glEnableVertexArrayAttrib;
PFNGLDISABLEVERTEXARRAYATTRIBPROC glDisableVertexArrayAttrib;
PFNGLGETERRORPROC glGetError;

#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126
#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

typedef void *(__stdcall *wglCreateContext_fn)(void *);
typedef i32 (__stdcall *wglDeleteContext_fn)(void *);
typedef void *(__stdcall *wglGetProcAddress_fn)(const char *);
typedef i32 (__stdcall *wglMakeCurrent_fn)(void *, void *);
typedef void *(__stdcall *wglCreateContextAttribsARB_fn)(void *, void *, const i32 *);
typedef i32 (__stdcall *wglSwapIntervalEXT_fn)(i32);

static wglCreateContext_fn s_wglCreateContext;
static wglDeleteContext_fn s_wglDeleteContext;
static wglGetProcAddress_fn s_wglGetProcAddress;
static wglMakeCurrent_fn s_wglMakeCurrent;
static wglCreateContextAttribsARB_fn s_wglCreateContextAttribsARB;
static wglSwapIntervalEXT_fn s_wglSwapIntervalEXT;

static void *s_device_context;
static void *s_opengl_dll;
static void *s_opengl_context;
static void *s_main_heap;

char *load_text_file(const char *filename)
{
    void *handle = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    assert(handle != (void *)-1);
    u32 size = GetFileSize(handle, NULL);
    char *content = (char *)malloc(size + 1);
    u32 bytes_read;
    ReadFile(handle, content, size, &bytes_read, NULL);
    CloseHandle(handle);
    assert(bytes_read == size);
    content[size] = '\0';
    return content;
}

void *malloc(u64 size)
{
    assert(size > 0);
    assert(s_main_heap);
    void *mem = HeapAlloc(s_main_heap, 0, size);
    if (!mem) {
        OutputDebugString("Failed to allocate memory!");
        assert(0);
        ExitProcess(1);
    }
    return mem;
}

void free(void *addr)
{
    assert(addr);
    assert(s_main_heap);
    if (!HeapFree(s_main_heap, 0, addr)) {
        OutputDebugString("Failed to free memory!");
        assert(0);
        ExitProcess(1);
    }
}

static void *get_opengl_func(const char *name)
{
    void *ptr = NULL;
    if (s_wglGetProcAddress) {
        ptr = s_wglGetProcAddress(name);
    }
    if (!ptr) {
        ptr = GetProcAddress(s_opengl_dll, name);
    }
    assert(ptr);
    return ptr;
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
    static unsigned frame_count;

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
        wsprintf(text, "[%d fps  %d us] %s", (int)fps, (int)us, name);
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

static void *init_opengl(const char *name, u32 width, u32 height)
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

    void *window = CreateWindowEx(
        0, name, name, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, NULL, 0);
    s_device_context = GetDC(window);

    PIXELFORMATDESCRIPTOR pfd = {
        .nSize = sizeof(PIXELFORMATDESCRIPTOR), .nVersion = 1,
        .dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW,
        .iPixelType = PFD_TYPE_RGBA, .cColorBits = 24, .cDepthBits = 24, .cStencilBits = 8,
    };
    SetPixelFormat(s_device_context, ChoosePixelFormat(s_device_context, &pfd), &pfd);


    s_opengl_dll = LoadLibraryA("opengl32.dll");
    s_wglCreateContext = (wglCreateContext_fn)GetProcAddress(s_opengl_dll, "wglCreateContext");
    s_wglMakeCurrent = (wglMakeCurrent_fn)GetProcAddress(s_opengl_dll, "wglMakeCurrent");
    s_wglGetProcAddress = (wglGetProcAddress_fn)GetProcAddress(s_opengl_dll, "wglGetProcAddress");
    s_wglDeleteContext = (wglDeleteContext_fn)GetProcAddress(s_opengl_dll, "wglDeleteContext");

    s_opengl_context = s_wglCreateContext(s_device_context);
    s_wglMakeCurrent(s_device_context, s_opengl_context);

    s_wglCreateContextAttribsARB = (wglCreateContextAttribsARB_fn)get_opengl_func("wglCreateContextAttribsARB");

    s_wglMakeCurrent(NULL, NULL);
    s_wglDeleteContext(s_opengl_context);

    i32 ctxattribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 5,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    s_opengl_context = s_wglCreateContextAttribsARB(s_device_context, NULL, ctxattribs);
    assert(s_opengl_context);

    s_wglMakeCurrent(s_device_context, s_opengl_context);

    s_wglSwapIntervalEXT = (wglSwapIntervalEXT_fn)get_opengl_func("wglSwapIntervalEXT");
    if (s_wglSwapIntervalEXT) {
        s_wglSwapIntervalEXT(0);
    }

    glCreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC)get_opengl_func("glCreateVertexArrays");
    glCreateProgramPipelines = (PFNGLCREATEPROGRAMPIPELINESPROC)get_opengl_func("glCreateProgramPipelines");
    glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC)get_opengl_func("glCreateShaderProgramv");
    glCreateTextures = (PFNGLCREATETEXTURESPROC)get_opengl_func("glCreateTextures");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)get_opengl_func("glDeleteVertexArrays");
    glDeleteProgramPipelines = (PFNGLDELETEPROGRAMPIPELINESPROC)get_opengl_func("glDeleteProgramPipelines");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)get_opengl_func("glDeleteProgram");
    glDeleteTextures = (PFNGLDELETETEXTURESPROC)get_opengl_func("glDeleteTextures");
    glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC)get_opengl_func("glUseProgramStages");
    glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC)get_opengl_func("glBindProgramPipeline");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)get_opengl_func("glBindVertexArray");
    glDrawArrays = (PFNGLDRAWARRAYSPROC)get_opengl_func("glDrawArrays");
    glDispatchCompute = (PFNGLDISPATCHCOMPUTEPROC)get_opengl_func("glDispatchCompute");
    glTextureStorage2D = (PFNGLTEXTURESTORAGE2DPROC)get_opengl_func("glTextureStorage2D");
    glTextureParameteri = (PFNGLTEXTUREPARAMETERIPROC)get_opengl_func("glTextureParameteri");
    glBindTextureUnit = (PFNGLBINDTEXTUREUNITPROC)get_opengl_func("glBindTextureUnit");
    glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC)get_opengl_func("glBindImageTexture");
    glCreateBuffers = (PFNGLCREATEBUFFERSPROC)get_opengl_func("glCreateBuffers");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)get_opengl_func("glDeleteBuffers");
    glNamedBufferStorage = (PFNGLNAMEDBUFFERSTORAGEPROC)get_opengl_func("glNamedBufferStorage");
    glVertexArrayAttribBinding = (PFNGLVERTEXARRAYATTRIBBINDINGPROC)get_opengl_func("glVertexArrayAttribBinding");
    glVertexArrayAttribFormat = (PFNGLVERTEXARRAYATTRIBFORMATPROC)get_opengl_func("glVertexArrayAttribFormat");
    glVertexArrayVertexBuffer = (PFNGLVERTEXARRAYVERTEXBUFFERPROC)get_opengl_func("glVertexArrayVertexBuffer");
    glEnableVertexArrayAttrib = (PFNGLENABLEVERTEXARRAYATTRIBPROC)get_opengl_func("glEnableVertexArrayAttrib");
    glDisableVertexArrayAttrib = (PFNGLDISABLEVERTEXARRAYATTRIBPROC)get_opengl_func("glDisableVertexArrayAttrib");
    glGetError = (PFNGLGETERRORPROC)get_opengl_func("glGetError");

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

    void *gdi32_dll = LoadLibraryA("gdi32.dll");
    SwapBuffers = GetProcAddress(gdi32_dll, "SwapBuffers");
    SetPixelFormat = GetProcAddress(gdi32_dll, "SetPixelFormat");
    ChoosePixelFormat = GetProcAddress(gdi32_dll, "ChoosePixelFormat");

    SetProcessDPIAware();
    s_main_heap = GetProcessHeap();

    void *window = init_opengl(k_demo_name, k_win_width, k_win_height);

    demo_init();

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

            demo_update(frame_time, frame_delta_time);
            demo_draw();

            SwapBuffers(s_device_context);
        }
    }

    demo_shutdown();

    s_wglMakeCurrent(NULL, NULL);
    s_wglDeleteContext(s_opengl_context);

    ExitProcess(0);
}
