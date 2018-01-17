#include "demo01.h"
#include "library.h"
#include "d3d12.h"


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
#define INFINITE 0xFFFFFFFF
#define STANDARD_RIGHTS_REQUIRED 0x000F0000L
#define SYNCHRONIZE 0x00100000L
#define EVENT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED|SYNCHRONIZE|0x3)
#define MEM_COMMIT 0x1000
#define MEM_RESERVE 0x2000
#define MEM_RELEASE 0x8000

#define PAGE_READWRITE 0x04

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

const GUID IID_ID3D12Debug = { 0x344488b7,0x6846,0x474b,0xb9,0x89,0xf0,0x27,0x44,0x82,0x45,0xe0 };
const GUID IID_IDXGISwapChain = { 0x310d36a0,0xd2e7,0x4c0a,0xaa,0x04,0x6a,0x9d,0x23,0xb8,0x88,0x6a };
const GUID IID_IDXGISwapChain3 = { 0x94d99bdb,0xf1f8,0x4ab0,0xb2,0x36,0x7d,0xa0,0x17,0x0e,0xda,0xb1 };
const GUID IID_IDXGIFactory4 = { 0x1bc6ea02,0xef36,0x464f,0xbf,0x0c,0x21,0xca,0x39,0xe5,0x16,0x8a };
const GUID IID_ID3D12GraphicsCommandList = { 0x5b160d0f,0xac1b,0x4185,0x8b,0xa8,0xb3,0xae,0x42,0xa5,0xa4,0x55 };
const GUID IID_ID3D12CommandQueue = { 0x0ec870a6,0x5d7e,0x4c22,0x8c,0xfc,0x5b,0xaa,0xe0,0x76,0x16,0xed };
const GUID IID_ID3D12Device = { 0x189819f1,0x1db6,0x4b57,0xbe,0x54,0x18,0x21,0x33,0x9b,0x85,0xf7 };
const GUID IID_ID3D12DescriptorHeap = { 0x8efb471d,0x616c,0x4f49,0x90,0xf7,0x12,0x7b,0xb7,0x63,0xfa,0x51 };
const GUID IID_ID3D12Resource = { 0x696442be,0xa72e,0x4059,0xbc,0x79,0x5b,0x5c,0x98,0x04,0x0f,0xad };
const GUID IID_ID3D12RootSignature = { 0xc54a6b66,0x72df,0x4ee8,0x8b,0xe5,0xa9,0x46,0xa1,0x42,0x92,0x14 };
const GUID IID_ID3D12CommandAllocator = { 0x6102dee4,0xaf59,0x4b09,0xb9,0x99,0xb4,0x4d,0x73,0xf0,0x9b,0x24 };
const GUID IID_ID3D12Fence = { 0x0a753dcf,0xc4d8,0x4b91,0xad,0xf6,0xbe,0x5a,0x60,0xd9,0x5a,0x76 };
const GUID IID_ID3D12PipelineState = { 0x765a30f3,0xf624,0x4c6f,0xa8,0x28,0xac,0xe9,0x48,0x62,0x24,0x45 };

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

static i32 (__stdcall *CreateDXGIFactory1)(const GUID *, void **);

static i32 (__stdcall *D3D12CreateDevice)(IUnknown *, D3D_FEATURE_LEVEL, const GUID *, void **);
static i32 (__stdcall *D3D12GetDebugInterface)(const GUID *, void **);

static void *s_main_heap;
static IDXGIFactory4 *s_dxgi_factory;
static ID3D12Device *s_d3d;
static ID3D12CommandQueue *s_cmdqueue;
static IDXGISwapChain3 *s_swapchain;
static u64 s_frame_count;
static u32 s_frame_index;
static ID3D12Fence *s_frame_fence;
static void *s_frame_fence_event;

char *load_file(const char *filename)
{
    void *handle = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
    assert(handle != (void *)-1);
    u32 size = GetFileSize(handle, NULL);
    char *content = (char *)malloc(size);
    u32 bytes_read;
    ReadFile(handle, content, size, &bytes_read, NULL);
    CloseHandle(handle);
    assert(bytes_read == size);
    return content;
}

void *malloc(size_t size)
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

void *realloc(void *addr, size_t size)
{
    assert(size > 0);
    assert(s_main_heap);
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

    void *window = CreateWindowEx(
        0, name, name, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, NULL, 0);

    return window;
}

static void present_frame(void)
{
    assert(s_cmdqueue);

    IDXGISwapChain3_Present(s_swapchain, 0, 0);
    ID3D12CommandQueue_Signal(s_cmdqueue, s_frame_fence, ++s_frame_count);

    u64 gpu_frame_count = ID3D12Fence_GetCompletedValue(s_frame_fence);

    if ((s_frame_count - gpu_frame_count) >= 2) {
        ID3D12Fence_SetEventOnCompletion(s_frame_fence, gpu_frame_count + 1, s_frame_fence_event);
        WaitForSingleObject(s_frame_fence_event, INFINITE);
    }

    s_frame_index = !s_frame_index;
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

    void *d3d12_dll = LoadLibraryA("d3d12.dll");
    if (!d3d12_dll) {
        // TODO: Add MessageBox
        ExitProcess(1);
    }
    D3D12CreateDevice = GetProcAddress(d3d12_dll, "D3D12CreateDevice");
    D3D12GetDebugInterface = GetProcAddress(d3d12_dll, "D3D12GetDebugInterface");

    void *dxgi_dll = LoadLibraryA("dxgi.dll");
    CreateDXGIFactory1 = GetProcAddress(dxgi_dll, "CreateDXGIFactory1");


    SetProcessDPIAware();
    s_main_heap = GetProcessHeap();

    // init window
    void *window = init_window(k_demo_name, k_win_width, k_win_height);

    // init d3d12
    VHR(CreateDXGIFactory1(&IID_IDXGIFactory4, &s_dxgi_factory));
#ifdef _DEBUG
    ID3D12Debug *dbg = NULL;
    D3D12GetDebugInterface(&IID_ID3D12Debug, &dbg);
    if (dbg) {
        ID3D12Debug_EnableDebugLayer(dbg);
        ID3D12Debug_Release(dbg);
    }
#endif
    if (D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_12_0, &IID_ID3D12Device, &s_d3d) != 0) {
        // TODO: D3D_FEATURE_LEVEL_12_0 not supported, display MessageBox
        return;
    }

    D3D12_COMMAND_QUEUE_DESC cmdqueue_desc = {
        .Flags = D3D12_COMMAND_QUEUE_FLAG_NONE, .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
        .Type = D3D12_COMMAND_LIST_TYPE_DIRECT
    };
    VHR(ID3D12Device_CreateCommandQueue(s_d3d, &cmdqueue_desc, &IID_ID3D12CommandQueue, &s_cmdqueue));

    DXGI_SWAP_CHAIN_DESC swapchain_desc = {
        .BufferCount = 4, .BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM, .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .OutputWindow = window, .SampleDesc.Count = 1, .SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL, .Windowed = 1
    };
    IDXGISwapChain *swapchain;
    VHR(IDXGIFactory4_CreateSwapChain(s_dxgi_factory, (IUnknown *)s_cmdqueue, &swapchain_desc, &swapchain));
    VHR(IDXGISwapChain_QueryInterface(swapchain, &IID_IDXGISwapChain3, &s_swapchain));
    IDXGISwapChain_Release(swapchain);

    VHR(ID3D12Device_CreateFence(s_d3d, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, &s_frame_fence));
    s_frame_fence_event = CreateEventEx(NULL, NULL, 0, EVENT_ALL_ACCESS);
    assert(s_frame_fence_event != NULL);

    // init demo
    demo_init(s_swapchain, s_d3d, s_cmdqueue);

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
            demo_draw(s_frame_index);

            present_frame();
        }
    }

    demo_shutdown();
    ExitProcess(0);
}
