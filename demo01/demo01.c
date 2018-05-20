#include "demo01.h"
#include "library.h"
#include "windows.h"


void * STDCALL LoadLibraryA(const char *module_name);
void * STDCALL GetProcAddress(void *module, const char *proc);

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

void (STDCALLP OutputDebugString)(const char *output_string);
i32 (STDCALLP QueryPerformanceCounter)(i64 *out_performance_count);
i32 (STDCALLP QueryPerformanceFrequency)(i64 *out_frequency);
void *(STDCALLP VirtualAlloc)(void *address, u64 size, u32 allocation_type, u32 protect);
i32 (STDCALLP VirtualFree)(void *address, u64 size, u32 free_type);
void (STDCALLP ExitProcess)(u32 exit_code);
void *(STDCALLP CreateFile)(const char *filename,
                            u32 desired_access,
                            u32 shared_mode,
                            SECURITY_ATTRIBUTES *security_attributes,
                            u32 creation_disposition,
                            u32 flags_and_attributes,
                            void *template_file);
i32 (STDCALLP ReadFile)(void *handle,
                        void *out_buffer,
                        u32 number_of_bytes_to_read,
                        u32 *out_number_of_bytes_read,
                        void *overlapped);
u32 (STDCALLP GetFileSize)(void *handle, u32 *out_file_size_high);
i32 (STDCALLP CloseHandle)(void *handle);
void *(STDCALLP GetModuleHandle)(const char *module_name);
void (STDCALLP Sleep)(u32 milliseconds);
void *(STDCALLP HeapAlloc)(void *heap, u32 flags, u64 bytes);
i32 (STDCALLP HeapFree)(void *heap, u32 flags, void *address);
void *(STDCALLP HeapReAlloc)(void *heap, u32 flags, void *address, u64 bytes);
void *(STDCALLP GetProcessHeap)(void);
void *(STDCALLP CreateEventEx)(SECURITY_ATTRIBUTES *event_attributes, const char *name, u32 flags, u32 desired_access);
u32 (STDCALLP WaitForSingleObject)(void *handle, u32 milliseconds);

i32 (STDCALLP PeekMessage)(MSG *out_msg, void *hwnd, u32 msg_filter_min, u32 msg_filter_max, u32 remove_msg);
i64 (STDCALLP DispatchMessage)(const MSG *msg);
void (STDCALLP PostQuitMessage)(i32 exit_code);
i64 (STDCALLP DefWindowProc)(void *hwnd, u32 msg, u64 wparam, i64 lparam);
void *(STDCALLP LoadCursor)(void *instance, const char *cursor_name);
i16 (STDCALLP RegisterClass)(const WNDCLASS *winclass);
void *(STDCALLP CreateWindowEx)(u32 exstyle,
                                const char *class_name,
                                const char *window_name,
                                u32 style,
                                i32 x,
                                i32 y,
                                i32 width,
                                i32 height,
                                void *parent,
                                void *menu,
                                void *instance,
                                void *param);
i32 (STDCALLP AdjustWindowRect)(RECT *out_rect, u32 style, i32 menu);
i32 (__cdecl *wsprintf)(char *out_string, const char *format, ...);
i32 (STDCALLP SetWindowText)(void *hwnd, const char *string);
i32 (STDCALLP SetProcessDPIAware)(void);
void *(STDCALLP GetDC)(void *hwnd);
i32 (STDCALLP MessageBox)(void *hwnd, const char *text, const char *caption, u32 type);
i32 (STDCALLP GetClientRect)(void *hwnd, RECT *out_rect);

i32 (STDCALLP CreateDXGIFactory1)(const GUID *guid, void **out_object);

i32 (STDCALLP D3D12CreateDevice)(IUnknown *adapter, D3D_FEATURE_LEVEL min_feature_level, const GUID *guid, void **out_object);
i32 (STDCALLP D3D12GetDebugInterface)(const GUID *guid, void **out_object);

static void update_frame_time(void *window, const char *name, f64 *time, f32 *delta_time)
{
    static f64 last_time;
    static f64 last_fps_time;
    static u32 frame_count;

    if (last_time == 0.0) {
        last_time = lib_get_time();
        last_fps_time = last_time;
    }

    *time = lib_get_time();
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

static i64 STDCALL process_window_message(void *window, u32 message, u64 wparam, i64 lparam)
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

static void *create_window(const char *name, u32 width, u32 height)
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
        MessageBox(NULL, "Program requires Windows 10 machine with DirectX 12 support (D3D_FEATURE_LEVEL_11_1 or better).",
                   "Error", 0);
        ExitProcess(1);
    }
    D3D12CreateDevice = GetProcAddress(d3d12_dll, "D3D12CreateDevice");
    D3D12GetDebugInterface = GetProcAddress(d3d12_dll, "D3D12GetDebugInterface");

    void *dxgi_dll = LoadLibraryA("dxgi.dll");
    CreateDXGIFactory1 = GetProcAddress(dxgi_dll, "CreateDXGIFactory1");

    SetProcessDPIAware();

    // init window
    void *window = create_window(k_demo_name, k_win_width, k_win_height);

    // init experiment
    experiment_dispatch_t dispatch = experiment01_init(window);

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

            dispatch.update(dispatch.experiment, frame_time, frame_delta_time);
            dispatch.draw(dispatch.experiment);
        }
    }

    dispatch.shutdown(dispatch.experiment);
    ExitProcess(0);
}
