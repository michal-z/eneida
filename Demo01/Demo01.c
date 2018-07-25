#define NULL ((void *)0)

#ifdef _DEBUG
#define assert(expression) if (!(expression)) { __debugbreak(); }
#else
#define assert(expression) {}
#endif

#define restrict __restrict
#define STDCALL __stdcall
#define STDCALLP STDCALL *

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef char i8;
typedef short i16;
typedef int i32;
typedef long long i64;
typedef float f32;
typedef double f64;
typedef u64 usize;
typedef i64 isize;
#define float __INVALID__
#define double __INVALID__
#define unsigned __INVALID__
#define int __INVALID__
#define short __INVALID__
#define long __INVALID__

static void (STDCALLP glCreateVertexArrays)(i32 N, u32 *Arrays);
static void (STDCALLP glCreateTextures)(u32 Target, i32 N, u32 *Textures);
static void (STDCALLP glCreateBuffers)(i32 N, u32 *Buffers);
static void (STDCALLP glDeleteVertexArrays)(i32 N, const u32 *Arrays);
static void (STDCALLP glDeleteProgram)(u32 Program);
static void (STDCALLP glDeleteTextures)(i32 N, const u32 *Textures);
static void (STDCALLP glDeleteBuffers)(i32 N, const u32 *Buffers);
static void (STDCALLP glBindVertexArray)(u32 Array);
static void (STDCALLP glDrawArrays)(u32 Mode, i32 First, i32 Count);
static void (STDCALLP glNamedBufferStorage)(u32 Buffer, isize Size, const void *Data, u32 Flags);
static void (STDCALLP glDispatchCompute)(u32 NumGroupsX, u32 NumGroupsY, u32 NumGroupsZ);
static void (STDCALLP glTextureStorage2D)(u32 Texture, i32 Levels, u32 InternalFormat, i32 Width, i32 Height);
static void (STDCALLP glTextureParameteri)(u32 Texture, u32 ParamName, i32 Param);
/*
PFNGLCREATEPROGRAMPIPELINESPROC glCreateProgramPipelines;
PFNGLCREATESHADERPROGRAMVPROC glCreateShaderProgramv;
PFNGLDELETEPROGRAMPIPELINESPROC glDeleteProgramPipelines;
PFNGLUSEPROGRAMSTAGESPROC glUseProgramStages;
PFNGLBINDPROGRAMPIPELINEPROC glBindProgramPipeline;

PFNGLBINDTEXTUREUNITPROC glBindTextureUnit;
PFNGLBINDIMAGETEXTUREPROC glBindImageTexture;
PFNGLVERTEXARRAYATTRIBBINDINGPROC glVertexArrayAttribBinding;
PFNGLVERTEXARRAYATTRIBFORMATPROC glVertexArrayAttribFormat;
PFNGLVERTEXARRAYVERTEXBUFFERPROC glVertexArrayVertexBuffer;
PFNGLENABLEVERTEXARRAYATTRIBPROC glEnableVertexArrayAttrib;
PFNGLDISABLEVERTEXARRAYATTRIBPROC glDisableVertexArrayAttrib;
PFNGLGETERRORPROC glGetError;
*/

//#include "demo01.h"
//#include "library.h"
//#include "windows.h"

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

typedef struct POINT
{
    i32 x;
    i32 y;
} POINT;

typedef struct MSG
{
    void *hwnd;
    u32 message;
    u64 wParam;
    i64 lParam;
    u32 time;
    POINT pt;
} MSG;

typedef struct WNDCLASS
{
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

typedef struct RECT
{
    i32 left;
    i32 top;
    i32 right;
    i32 bottom;
} RECT;

typedef struct SECURITY_ATTRIBUTES
{
    u32 nLength;
    void *lpSecurityDescriptor;
    i32 bInheritHandle;
} SECURITY_ATTRIBUTES;

typedef struct PIXELFORMATDESCRIPTOR
{
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

extern void *STDCALL LoadLibraryA(const char *ModuleName);
extern void *STDCALL GetProcAddress(void *Module, const char *ProcName);

void *memset(void *Dest, i32 Value, usize Count);
#pragma intrinsic(memset)

i32 _fltused;

static void (STDCALLP OutputDebugString)(const char *OutputString);
static i32 (STDCALLP QueryPerformanceCounter)(i64 *PerformanceCounter);
static i32 (STDCALLP QueryPerformanceFrequency)(i64 *Frequency);
static void *(STDCALLP VirtualAlloc)(void *Address, u64 Size, u32 AllocationType, u32 Protect);
static i32 (STDCALLP VirtualFree)(void *Address, u64 Size, u32 FreeType);
static void (STDCALLP ExitProcess)(u32 ExitCode);
static void *(STDCALLP CreateFile)(const char *FileName,
                                   u32 DesiredAccess,
                                   u32 SharedMode,
                                   SECURITY_ATTRIBUTES *SecurityAttributes,
                                   u32 CreationDisposition,
                                   u32 FlagsAndAttributes,
                                   void *TemplateFile);
static i32 (STDCALLP ReadFile)(void *Handle,
                               void *Buffer,
                               u32 NumberOfBytesToRead,
                               u32 *NumberOfBytesRead,
                               void *Overlapped);
static u32 (STDCALLP GetFileSize)(void *Handle, u32 *FileSizeHigh);
static i32 (STDCALLP CloseHandle)(void *Handle);
static void *(STDCALLP GetModuleHandle)(const char *ModuleName);
static void (STDCALLP Sleep)(u32 MilliSeconds);
static void *(STDCALLP HeapAlloc)(void *Heap, u32 Flags, u64 Bytes);
static i32 (STDCALLP HeapFree)(void *Heap, u32 Flags, void *Address);
static void *(STDCALLP HeapReAlloc)(void *Heap, u32 Flags, void *Address, u64 Bytes);
static void *(STDCALLP GetProcessHeap)(void);
static void *(STDCALLP CreateEventEx)(SECURITY_ATTRIBUTES *EventAttributes, const char *Name, u32 Flags, u32 DesiredAccess);
static u32 (STDCALLP WaitForSingleObject)(void *Handle, u32 MilliSeconds);

static i32 (STDCALLP PeekMessage)(MSG *Msg, void *Hwnd, u32 MsgFilterMin, u32 MsgFilterMax, u32 RemoveMsg);
static i64 (STDCALLP DispatchMessage)(const MSG *Msg);
static void (STDCALLP PostQuitMessage)(i32 ExitCode);
static i64 (STDCALLP DefWindowProc)(void *Hwnd, u32 Msg, u64 WParam, i64 LParam);
static void *(STDCALLP LoadCursor)(void *Instance, const char *CursorName);
static i16 (STDCALLP RegisterClass)(const WNDCLASS *WinClass);
static void *(STDCALLP CreateWindowEx)(u32 ExStyle,
                                       const char *ClassName,
                                       const char *WindowName,
                                       u32 Style,
                                       i32 X,
                                       i32 Y,
                                       i32 Width,
                                       i32 Height,
                                       void *Parent,
                                       void *Menu,
                                       void *Instance,
                                       void *Param);
static i32 (STDCALLP AdjustWindowRect)(RECT *Rect, u32 Style, i32 Menu);
static i32 (__cdecl *wsprintf)(char *String, const char *Format, ...);
static i32 (STDCALLP SetWindowText)(void *Hwnd, const char *String);
static i32 (STDCALLP SetProcessDPIAware)(void);
static void *(STDCALLP GetDC)(void *Hwnd);
static i32 (STDCALLP MessageBox)(void *Hwnd, const char *Text, const char *Caption, u32 Type);
static i32 (STDCALLP GetClientRect)(void *Hwnd, RECT *Rect);

static i32 (STDCALLP SwapBuffers)(void *DeviceContext);
static i32 (STDCALLP SetPixelFormat)(void *DeviceContext, i32 PixelFormat, const PIXELFORMATDESCRIPTOR *PixelFormatDesc);
static i32 (STDCALLP ChoosePixelFormat)(void *DeviceContext, const PIXELFORMATDESCRIPTOR *PixelFormatDesc);

#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126
#define WGL_CONTEXT_DEBUG_BIT_ARB               0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB  0x0002
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

typedef struct windows_context
{
    const char *Name;
    void *Window;
    void *OpenGL32Dll;
    void *DeviceContext;
    void *GraphicsContext;
    void *(STDCALLP wglCreateContext)(void *DeviceContext);
    i32 (STDCALLP wglDeleteContext)(void *GraphicsContext);
    void *(STDCALLP wglGetProcAddress)(const char *ProcName);
    i32 (STDCALLP wglMakeCurrent)(void *DeviceContext, void *GraphicsContext);
    void *(STDCALLP wglCreateContextAttribsARB)(void *DeviceContext, void *GraphicsContext, const i32 *Attribs);
    i32 (STDCALLP wglSwapIntervalEXT)(i32 Interval);
} windows_context;

#pragma function(memset)
void *
memset(void *Dest, i32 Value, usize Count)
{
    __stosb((u8 *)Dest, (u8)Value, Count);
    return Dest;
}

static void *
GetFunctionOpenGL(const windows_context *Ctx, const char *FunctionName)
{
    void *Ptr = NULL;
    if (Ctx->wglGetProcAddress)
    {
        Ptr = Ctx->wglGetProcAddress(FunctionName);
    }
    if (!Ptr)
    {
        Ptr = GetProcAddress(Ctx->OpenGL32Dll, FunctionName);
    }
    assert(Ptr);
    return Ptr;
}

static f64
GetGlobalTime(void)
{
    static i64 Frequency, StartCounter;
    if (Frequency == 0)
    {
        QueryPerformanceCounter(&StartCounter);
        QueryPerformanceFrequency(&Frequency);
    }

    i64 Counter;
    QueryPerformanceCounter(&Counter);
    return (Counter - StartCounter) / (f64)Frequency;
}

static void
UpdateFrameStats(void *Window, const char *Name, f64 *Time, f32 *DeltaTime)
{
    static f64 PreviousTime;
    static f64 HeaderRefreshTime;
    static u32 FrameCount;

    if (PreviousTime == 0.0)
    {
        PreviousTime = GetGlobalTime();
        HeaderRefreshTime = PreviousTime;
    }

    *Time = GetGlobalTime();
    *DeltaTime = (f32)(*Time - PreviousTime);
    PreviousTime = *Time;

    if ((*Time - HeaderRefreshTime) >= 1.0)
    {
        f64 FramesPerSecond = FrameCount / (*Time - HeaderRefreshTime);
        f64 MicroSeconds = (1.0 / FramesPerSecond) * 1000000.0;
        char Header[256];
        wsprintf(Header, "[%d fps  %d us] %s", (i32)FramesPerSecond, (i32)MicroSeconds, Name);
        SetWindowText(Window, Header);
        HeaderRefreshTime = *Time;
        FrameCount = 0;
    }
    FrameCount++;
}

static i64 STDCALL
ProcessWindowMessage(void *Window, u32 Message, u64 WParam, i64 LParam)
{
    switch (Message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (WParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
            return 0;
        }
        break;
    }
    return DefWindowProc(Window, Message, WParam, LParam);
}

static void
InitializeOpenGL(windows_context *Ctx)
{
    Ctx->DeviceContext = GetDC(Ctx->Window);
    assert(Ctx->DeviceContext);

    PIXELFORMATDESCRIPTOR PixelFormatDesc = {
        .nSize = sizeof(PIXELFORMATDESCRIPTOR),
        .nVersion = 1,
        .dwFlags = PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_DRAW_TO_WINDOW,
        .iPixelType = PFD_TYPE_RGBA,
        .cColorBits = 24,
        .cDepthBits = 24,
        .cStencilBits = 8,
    };
    SetPixelFormat(Ctx->DeviceContext, ChoosePixelFormat(Ctx->DeviceContext, &PixelFormatDesc), &PixelFormatDesc);

    Ctx->OpenGL32Dll = LoadLibraryA("opengl32.dll");
    Ctx->wglCreateContext = GetProcAddress(Ctx->OpenGL32Dll, "wglCreateContext");
    Ctx->wglMakeCurrent = GetProcAddress(Ctx->OpenGL32Dll, "wglMakeCurrent");
    Ctx->wglGetProcAddress = GetProcAddress(Ctx->OpenGL32Dll, "wglGetProcAddress");
    Ctx->wglDeleteContext = GetProcAddress(Ctx->OpenGL32Dll, "wglDeleteContext");

    Ctx->GraphicsContext = Ctx->wglCreateContext(Ctx->DeviceContext);
    Ctx->wglMakeCurrent(Ctx->DeviceContext, Ctx->GraphicsContext);

    Ctx->wglCreateContextAttribsARB = GetFunctionOpenGL(Ctx, "wglCreateContextAttribsARB");

    Ctx->wglMakeCurrent(NULL, NULL);
    Ctx->wglDeleteContext(Ctx->GraphicsContext);

    const i32 Attribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 5,
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
        0
    };
    Ctx->GraphicsContext = Ctx->wglCreateContextAttribsARB(Ctx->DeviceContext, NULL, Attribs);
    assert(Ctx->GraphicsContext);

    Ctx->wglMakeCurrent(Ctx->DeviceContext, Ctx->GraphicsContext);

    Ctx->wglSwapIntervalEXT = GetFunctionOpenGL(Ctx, "wglSwapIntervalEXT");
    if (Ctx->wglSwapIntervalEXT)
    {
        Ctx->wglSwapIntervalEXT(0);
    }

    /*
    glCreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC)GetFunctionOpenGL(Ctx, "glCreateVertexArrays");
    glCreateProgramPipelines = (PFNGLCREATEPROGRAMPIPELINESPROC)GetFunctionOpenGL(Ctx, "glCreateProgramPipelines");
    glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC)GetFunctionOpenGL(Ctx, "glCreateShaderProgramv");
    glCreateTextures = (PFNGLCREATETEXTURESPROC)GetFunctionOpenGL(Ctx, "glCreateTextures");
    glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)GetFunctionOpenGL(Ctx, "glDeleteVertexArrays");
    glDeleteProgramPipelines = (PFNGLDELETEPROGRAMPIPELINESPROC)GetFunctionOpenGL(Ctx, "glDeleteProgramPipelines");
    glDeleteProgram = (PFNGLDELETEPROGRAMPROC)GetFunctionOpenGL(Ctx, "glDeleteProgram");
    glDeleteTextures = (PFNGLDELETETEXTURESPROC)GetFunctionOpenGL(Ctx, "glDeleteTextures");
    glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC)GetFunctionOpenGL(Ctx, "glUseProgramStages");
    glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC)GetFunctionOpenGL(Ctx, "glBindProgramPipeline");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)GetFunctionOpenGL(Ctx, "glBindVertexArray");
    glDrawArrays = (PFNGLDRAWARRAYSPROC)GetFunctionOpenGL(Ctx, "glDrawArrays");
    glDispatchCompute = (PFNGLDISPATCHCOMPUTEPROC)GetFunctionOpenGL(Ctx, "glDispatchCompute");
    glTextureStorage2D = (PFNGLTEXTURESTORAGE2DPROC)GetFunctionOpenGL(Ctx, "glTextureStorage2D");
    glTextureParameteri = (PFNGLTEXTUREPARAMETERIPROC)GetFunctionOpenGL(Ctx, "glTextureParameteri");
    glBindTextureUnit = (PFNGLBINDTEXTUREUNITPROC)GetFunctionOpenGL(Ctx, "glBindTextureUnit");
    glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC)GetFunctionOpenGL(Ctx, "glBindImageTexture");
    glCreateBuffers = (PFNGLCREATEBUFFERSPROC)GetFunctionOpenGL(Ctx, "glCreateBuffers");
    glDeleteBuffers = (PFNGLDELETEBUFFERSPROC)GetFunctionOpenGL(Ctx, "glDeleteBuffers");
    glNamedBufferStorage = (PFNGLNAMEDBUFFERSTORAGEPROC)GetFunctionOpenGL(Ctx, "glNamedBufferStorage");
    glVertexArrayAttribBinding = (PFNGLVERTEXARRAYATTRIBBINDINGPROC)GetFunctionOpenGL(Ctx, "glVertexArrayAttribBinding");
    glVertexArrayAttribFormat = (PFNGLVERTEXARRAYATTRIBFORMATPROC)GetFunctionOpenGL(Ctx, "glVertexArrayAttribFormat");
    glVertexArrayVertexBuffer = (PFNGLVERTEXARRAYVERTEXBUFFERPROC)GetFunctionOpenGL(Ctx, "glVertexArrayVertexBuffer");
    glEnableVertexArrayAttrib = (PFNGLENABLEVERTEXARRAYATTRIBPROC)GetFunctionOpenGL(Ctx, "glEnableVertexArrayAttrib");
    glDisableVertexArrayAttrib = (PFNGLDISABLEVERTEXARRAYATTRIBPROC)GetFunctionOpenGL(Ctx, "glDisableVertexArrayAttrib");
    glGetError = (PFNGLGETERRORPROC)GetFunctionOpenGL(Ctx, "glGetError");
    */
}

static void *
InitializeWindow(const char *Name, u32 Width, u32 Height)
{
    WNDCLASS WinClass = {
        .lpfnWndProc = ProcessWindowMessage,
        .hInstance = GetModuleHandle(NULL),
        .hCursor = LoadCursor(NULL, IDC_ARROW),
        .lpszClassName = Name,
    };
    if (!RegisterClass(&WinClass))
        assert(0);

    RECT Rect = { 0, 0, (i32)Width, (i32)Height };
    if (!AdjustWindowRect(&Rect, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, 0))
        assert(0);

    void *Window = CreateWindowEx(
        0, Name, Name, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        Rect.right - Rect.left, Rect.bottom - Rect.top,
        NULL, NULL, NULL, 0);

    return Window;
}

void
Start(void)
{
    void *Kernel32Dll = LoadLibraryA("kernel32.dll");
    OutputDebugString = GetProcAddress(Kernel32Dll, "OutputDebugStringA");
    GetModuleHandle = GetProcAddress(Kernel32Dll, "GetModuleHandleA");
    QueryPerformanceCounter = GetProcAddress(Kernel32Dll, "QueryPerformanceCounter");
    QueryPerformanceFrequency = GetProcAddress(Kernel32Dll, "QueryPerformanceFrequency");
    VirtualAlloc = GetProcAddress(Kernel32Dll, "VirtualAlloc");
    VirtualFree = GetProcAddress(Kernel32Dll, "VirtualFree");
    ExitProcess = GetProcAddress(Kernel32Dll, "ExitProcess");
    CreateFile = GetProcAddress(Kernel32Dll, "CreateFileA");
    ReadFile = GetProcAddress(Kernel32Dll, "ReadFile");
    GetFileSize = GetProcAddress(Kernel32Dll, "GetFileSize");
    CloseHandle = GetProcAddress(Kernel32Dll, "CloseHandle");
    Sleep = GetProcAddress(Kernel32Dll, "Sleep");
    HeapAlloc = GetProcAddress(Kernel32Dll, "HeapAlloc");
    HeapFree = GetProcAddress(Kernel32Dll, "HeapFree");
    HeapReAlloc = GetProcAddress(Kernel32Dll, "HeapReAlloc");
    GetProcessHeap = GetProcAddress(Kernel32Dll, "GetProcessHeap");
    CreateEventEx = GetProcAddress(Kernel32Dll, "CreateEventExA");
    WaitForSingleObject = GetProcAddress(Kernel32Dll, "WaitForSingleObject");

    void *User32Dll = LoadLibraryA("user32.dll");
    PeekMessage = GetProcAddress(User32Dll, "PeekMessageA");
    DispatchMessage = GetProcAddress(User32Dll, "DispatchMessageA");
    PostQuitMessage = GetProcAddress(User32Dll, "PostQuitMessage");
    DefWindowProc = GetProcAddress(User32Dll, "DefWindowProcA");
    LoadCursor = GetProcAddress(User32Dll, "LoadCursorA");
    RegisterClass = GetProcAddress(User32Dll, "RegisterClassA");
    CreateWindowEx = GetProcAddress(User32Dll, "CreateWindowExA");
    AdjustWindowRect = GetProcAddress(User32Dll, "AdjustWindowRect");
    wsprintf = GetProcAddress(User32Dll, "wsprintfA");
    SetWindowText = GetProcAddress(User32Dll, "SetWindowTextA");
    SetProcessDPIAware = GetProcAddress(User32Dll, "SetProcessDPIAware");
    GetDC = GetProcAddress(User32Dll, "GetDC");
    MessageBox = GetProcAddress(User32Dll, "MessageBoxA");
    GetClientRect = GetProcAddress(User32Dll, "GetClientRect");

    void *Gdi32Dll = LoadLibraryA("gdi32.dll");
    SwapBuffers = GetProcAddress(Gdi32Dll, "SwapBuffers");
    SetPixelFormat = GetProcAddress(Gdi32Dll, "SetPixelFormat");
    ChoosePixelFormat = GetProcAddress(Gdi32Dll, "ChoosePixelFormat");

    SetProcessDPIAware();

    windows_context WindowsContext = { "Demo64k" };
    WindowsContext.Window = InitializeWindow(WindowsContext.Name, 1920, 1080);

    InitializeOpenGL(&WindowsContext);

    for (;;)
    {
        MSG Message = { 0 };
        if (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
            DispatchMessage(&Message);
            if (Message.message == WM_QUIT)
                break;
        }
        else
        {
            f64 FrameTime;
            f32 FrameDeltaTime;
            UpdateFrameStats(WindowsContext.Window, WindowsContext.Name, &FrameTime, &FrameDeltaTime);

            //demo_update(frame_time, frame_delta_time);
            //demo_draw();

            SwapBuffers(WindowsContext.DeviceContext);
        }
    }

    //demo_shutdown();

    WindowsContext.wglMakeCurrent(NULL, NULL);
    WindowsContext.wglDeleteContext(WindowsContext.GraphicsContext);

    ExitProcess(0);
}
