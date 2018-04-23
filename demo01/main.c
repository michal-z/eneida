#include "demo01.h"
#include "library.h"
#include "windows.h"


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

void (__stdcall *OutputDebugString)(const char *);
i32 (__stdcall *QueryPerformanceCounter)(i64 *);
i32 (__stdcall *QueryPerformanceFrequency)(i64 *);
void *(__stdcall *VirtualAlloc)(void *, u64, u32, u32);
i32 (__stdcall *VirtualFree)(void *, u64, u32);
void (__stdcall *ExitProcess)(u32);
void *(__stdcall *CreateFile)(const char *, u32, u32, SECURITY_ATTRIBUTES *, u32, u32, void *);
i32 (__stdcall *ReadFile)(void *, void *, u32, u32 *, void *);
u32 (__stdcall *GetFileSize)(void *, u32 *);
i32 (__stdcall *CloseHandle)(void *);
void *(__stdcall *GetModuleHandle)(const char *);
void (__stdcall *Sleep)(u32);
void *(__stdcall *HeapAlloc)(void *, u32, u64);
i32 (__stdcall *HeapFree)(void *, u32, void *);
void *(__stdcall *HeapReAlloc)(void *, u32, void *, u64);
void *(__stdcall *GetProcessHeap)(void);
void *(__stdcall *CreateEventEx)(SECURITY_ATTRIBUTES *, const char *, u32, u32);
u32 (__stdcall *WaitForSingleObject)(void *, u32);

i32 (__stdcall *PeekMessage)(MSG *, void *, u32, u32, u32);
i64 (__stdcall *DispatchMessage)(const MSG *);
void (__stdcall *PostQuitMessage)(i32);
i64 (__stdcall *DefWindowProc)(void *, u32, u64, i64);
void *(__stdcall *LoadCursor)(void *, const char *);
i16 (__stdcall *RegisterClass)(const WNDCLASS *);
void *(__stdcall *CreateWindowEx)(u32, const char *, const char *, u32, i32, i32, i32, i32, void *, void *, void *, void *);
i32 (__stdcall *AdjustWindowRect)(RECT *, u32, i32);
i32 (__cdecl *wsprintf)(char *, const char *, ...);
i32 (__stdcall *SetWindowText)(void *, const char *);
i32 (__stdcall *SetProcessDPIAware)(void);
void *(__stdcall *GetDC)(void *);
i32 (__stdcall *MessageBox)(void *, const char *, const char *, u32);
i32 (__stdcall *GetClientRect)(void *, RECT *);

i32 (__stdcall *CreateDXGIFactory1)(const GUID *, void **);

i32 (__stdcall *D3D12CreateDevice)(IUnknown *, D3D_FEATURE_LEVEL, const GUID *, void **);
i32 (__stdcall *D3D12GetDebugInterface)(const GUID *, void **);

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

static f64 get_time(void)
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

static void update_frame_time(void *window, const char *name, f64 *time, f32 *delta_time)
{
    static f64 last_time;
    static f64 last_fps_time;
    static u32 frame_count;

    if (last_time == 0.0) {
        last_time = get_time();
        last_fps_time = last_time;
    }

    *time = get_time();
    *delta_time = (f32)(*time - last_time);
    last_time = *time;

    if ((*time - last_fps_time) >= 1.0) {
        f64 fps = frame_count / (*time - last_fps_time);
        f64 us = (1.0 / fps) * 1000000.0;
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

    void *window = CreateWindowEx(
        0, name, name, WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left, rect.bottom - rect.top,
        NULL, NULL, NULL, 0);

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
    GetClientRect = GetProcAddress(user32_dll, "GetClientRect");

    void *d3d12_dll = LoadLibraryA("d3d12.dll");
    if (!d3d12_dll) {
        MessageBox(NULL, "Program requires Windows 10 machine with DirectX 12 support (D3D_FEATURE_LEVEL_11_1 or better).", "Error", 0);
        ExitProcess(1);
    }
    D3D12CreateDevice = GetProcAddress(d3d12_dll, "D3D12CreateDevice");
    D3D12GetDebugInterface = GetProcAddress(d3d12_dll, "D3D12GetDebugInterface");

    void *dxgi_dll = LoadLibraryA("dxgi.dll");
    CreateDXGIFactory1 = GetProcAddress(dxgi_dll, "CreateDXGIFactory1");

    SetProcessDPIAware();

    // init window
    void *window = init_window(k_demo_name, k_win_width, k_win_height);

    // init demo
    demo_t *demo = demo_init(window);

    for (;;) {
        MSG message = { 0 };
        if (PeekMessage(&message, 0, 0, 0, PM_REMOVE)) {
            DispatchMessage(&message);
            if (message.message == WM_QUIT)
                break;
        } else {
            f64 frame_time;
            f32 frame_delta_time;
            update_frame_time(window, k_demo_name, &frame_time, &frame_delta_time);

            demo_update(demo, frame_time, frame_delta_time);
            demo_draw(demo);
        }
    }

    demo_shutdown(demo);
    ExitProcess(0);
}
