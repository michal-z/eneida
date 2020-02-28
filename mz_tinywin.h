#ifndef MZ_TINYWIN_INCLUDED_
#define MZ_TINYWIN_INCLUDED_

#include "mz_fwd.h"

#define FALSE 0
#define TRUE 1
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
#define MEM_COMMIT 0x1000
#define MEM_RESERVE 0x2000
#define MEM_RELEASE 0x8000
#define PAGE_READWRITE 0x04
#define INFINITE 0xFFFFFFFF
#define STANDARD_RIGHTS_REQUIRED 0x000F0000L
#define SYNCHRONIZE 0x00100000L
#define EVENT_ALL_ACCESS (STANDARD_RIGHTS_REQUIRED | SYNCHRONIZE | 0x3)
#define MB_OK 0x00000000L
#define MB_ICONERROR 0x00000010L

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

typedef struct GUID {
  u32 Data1;
  u16 Data2;
  u16 Data3;
  u8 Data4[8];
} GUID;

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

typedef void(__stdcall *OutputDebugString_t)(const char *output_string);
typedef i32(__stdcall *QueryPerformanceCounter_t)(i64 *out_performance_count);
typedef i32(__stdcall *QueryPerformanceFrequency_t)(i64 *out_frequency);
typedef void *(__stdcall *VirtualAlloc_t)(void *address, u64 size,
                                          u32 allocation_type, u32 protect);
typedef i32(__stdcall *VirtualFree_t)(void *address, u64 size, u32 free_type);
typedef void(__stdcall *ExitProcess_t)(u32 exit_code);
typedef void *(__stdcall *CreateFile_t)(
    const char *filename, u32 desired_access, u32 shared_mode,
    SECURITY_ATTRIBUTES *security_attributes, u32 creation_disposition,
    u32 flags_and_attributes, void *template_file);
typedef i32(__stdcall *ReadFile_t)(void *handle, void *out_buffer,
                                   u32 number_of_bytes_to_read,
                                   u32 *out_number_of_bytes_read,
                                   void *overlapped);
typedef u32(__stdcall *GetFileSize_t)(void *handle, u32 *out_file_size_high);
typedef i32(__stdcall *CloseHandle_t)(void *handle);
typedef void *(__stdcall *GetModuleHandle_t)(const char *module_name);
typedef void(__stdcall *Sleep_t)(u32 milliseconds);
typedef void *(__stdcall *HeapAlloc_t)(void *heap, u32 flags, u64 bytes);
typedef i32(__stdcall *HeapFree_t)(void *heap, u32 flags, void *address);
typedef void *(__stdcall *HeapReAlloc_t)(void *heap, u32 flags, void *address,
                                         u64 bytes);
typedef void *(__stdcall *GetProcessHeap_t)(void);
typedef void *(__stdcall *CreateEventEx_t)(
    SECURITY_ATTRIBUTES *event_attributes, const char *name, u32 flags,
    u32 desired_access);
typedef u32(__stdcall *WaitForSingleObject_t)(void *handle, u32 milliseconds);

typedef i32(__stdcall *PeekMessage_t)(MSG *out_msg, void *hwnd,
                                      u32 msg_filter_min, u32 msg_filter_max,
                                      u32 remove_msg);
typedef i64(__stdcall *DispatchMessage_t)(const MSG *msg);
typedef void(__stdcall *PostQuitMessage_t)(i32 exit_code);
typedef i64(__stdcall *DefWindowProc_t)(void *hwnd, u32 msg, u64 wparam,
                                        i64 lparam);
typedef void *(__stdcall *LoadCursor_t)(void *instance,
                                        const char *cursor_name);
typedef i16(__stdcall *RegisterClass_t)(const WNDCLASS *winclass);
typedef void *(__stdcall *CreateWindowEx_t)(u32 exstyle, const char *class_name,
                                            const char *window_name, u32 style,
                                            i32 x, i32 y, i32 width, i32 height,
                                            void *parent, void *menu,
                                            void *instance, void *param);
typedef i32(__stdcall *AdjustWindowRect_t)(RECT *out_rect, u32 style, i32 menu);
typedef i32(__cdecl *wsprintf_t)(char *out_string, const char *format, ...);
typedef i32(__stdcall *SetWindowText_t)(void *hwnd, const char *string);
typedef i32(__stdcall *SetProcessDPIAware_t)(void);
typedef i32(__stdcall *MessageBox_t)(void *hwnd, const char *text,
                                     const char *caption, u32 type);
typedef i32(__stdcall *GetClientRect_t)(void *hwnd, RECT *out_rect);

extern GetModuleHandle_t GetModuleHandle;
extern OutputDebugString_t OutputDebugString;
extern QueryPerformanceCounter_t QueryPerformanceCounter;
extern QueryPerformanceFrequency_t QueryPerformanceFrequency;
extern ExitProcess_t ExitProcess;
extern VirtualAlloc_t VirtualAlloc;
extern VirtualFree_t VirtualFree;
extern CreateFile_t CreateFile;
extern ReadFile_t ReadFile;
extern GetFileSize_t GetFileSize;
extern CloseHandle_t CloseHandle;
extern Sleep_t Sleep;
extern HeapAlloc_t HeapAlloc;
extern HeapFree_t HeapFree;
extern HeapReAlloc_t HeapReAlloc;
extern GetProcessHeap_t GetProcessHeap;
extern CreateEventEx_t CreateEventEx;
extern WaitForSingleObject_t WaitForSingleObject;

extern PeekMessage_t PeekMessage;
extern DispatchMessage_t DispatchMessage;
extern PostQuitMessage_t PostQuitMessage;
extern DefWindowProc_t DefWindowProc;
extern LoadCursor_t LoadCursor;
extern RegisterClass_t RegisterClass;
extern CreateWindowEx_t CreateWindowEx;
extern AdjustWindowRect_t AdjustWindowRect;
extern wsprintf_t wsprintf;
extern SetWindowText_t SetWindowText;
extern SetProcessDPIAware_t SetProcessDPIAware;
extern MessageBox_t MessageBox;
extern GetClientRect_t GetClientRect;

void mztinywin_load_api(void);

#endif // #ifndef MZ_TINYWIN_INCLUDED_

#ifdef MZ_TINYWIN_IMPLEMENTATION

GetModuleHandle_t GetModuleHandle;
OutputDebugString_t OutputDebugString;
QueryPerformanceCounter_t QueryPerformanceCounter;
QueryPerformanceFrequency_t QueryPerformanceFrequency;
ExitProcess_t ExitProcess;
VirtualAlloc_t VirtualAlloc;
VirtualFree_t VirtualFree;
CreateFile_t CreateFile;
ReadFile_t ReadFile;
GetFileSize_t GetFileSize;
CloseHandle_t CloseHandle;
Sleep_t Sleep;
HeapAlloc_t HeapAlloc;
HeapFree_t HeapFree;
HeapReAlloc_t HeapReAlloc;
GetProcessHeap_t GetProcessHeap;
CreateEventEx_t CreateEventEx;
WaitForSingleObject_t WaitForSingleObject;

PeekMessage_t PeekMessage;
DispatchMessage_t DispatchMessage;
PostQuitMessage_t PostQuitMessage;
DefWindowProc_t DefWindowProc;
LoadCursor_t LoadCursor;
RegisterClass_t RegisterClass;
CreateWindowEx_t CreateWindowEx;
AdjustWindowRect_t AdjustWindowRect;
wsprintf_t wsprintf;
SetWindowText_t SetWindowText;
SetProcessDPIAware_t SetProcessDPIAware;
MessageBox_t MessageBox;
GetClientRect_t GetClientRect;

void mztinywin_load_api(void) {
  void *__stdcall LoadLibraryA(const char *dll_name);
  void *__stdcall GetProcAddress(void *dll, const char *proc);

  void *kernel32_dll = LoadLibraryA("kernel32.dll");
  GetModuleHandle =
      (GetModuleHandle_t)GetProcAddress(kernel32_dll, "GetModuleHandleA");
  OutputDebugString =
      (OutputDebugString_t)GetProcAddress(kernel32_dll, "OutputDebugStringA");
  QueryPerformanceCounter = (QueryPerformanceCounter_t)GetProcAddress(
      kernel32_dll, "QueryPerformanceCounter");
  QueryPerformanceFrequency = (QueryPerformanceFrequency_t)GetProcAddress(
      kernel32_dll, "QueryPerformanceFrequency");
  ExitProcess = (ExitProcess_t)GetProcAddress(kernel32_dll, "ExitProcess");
  VirtualAlloc = (VirtualAlloc_t)GetProcAddress(kernel32_dll, "VirtualAlloc");
  VirtualFree = (VirtualFree_t)GetProcAddress(kernel32_dll, "VirtualFree");
  CreateFile = (CreateFile_t)GetProcAddress(kernel32_dll, "CreateFileA");
  ReadFile = (ReadFile_t)GetProcAddress(kernel32_dll, "ReadFile");
  GetFileSize = (GetFileSize_t)GetProcAddress(kernel32_dll, "GetFileSize");
  CloseHandle = (CloseHandle_t)GetProcAddress(kernel32_dll, "CloseHandle");
  Sleep = (Sleep_t)GetProcAddress(kernel32_dll, "Sleep");
  HeapAlloc = (HeapAlloc_t)GetProcAddress(kernel32_dll, "HeapAlloc");
  HeapFree = (HeapFree_t)GetProcAddress(kernel32_dll, "HeapFree");
  HeapReAlloc = (HeapReAlloc_t)GetProcAddress(kernel32_dll, "HeapReAlloc");
  GetProcessHeap =
      (GetProcessHeap_t)GetProcAddress(kernel32_dll, "GetProcessHeap");
  CreateEventEx =
      (CreateEventEx_t)GetProcAddress(kernel32_dll, "CreateEventExA");
  WaitForSingleObject = (WaitForSingleObject_t)GetProcAddress(
      kernel32_dll, "WaitForSingleObject");

  void *user32_dll = LoadLibraryA("user32.dll");
  PeekMessage = (PeekMessage_t)GetProcAddress(user32_dll, "PeekMessageA");
  DispatchMessage =
      (DispatchMessage_t)GetProcAddress(user32_dll, "DispatchMessageA");
  PostQuitMessage =
      (PostQuitMessage_t)GetProcAddress(user32_dll, "PostQuitMessage");
  DefWindowProc = (DefWindowProc_t)GetProcAddress(user32_dll, "DefWindowProcA");
  LoadCursor = (LoadCursor_t)GetProcAddress(user32_dll, "LoadCursorA");
  RegisterClass = (RegisterClass_t)GetProcAddress(user32_dll, "RegisterClassA");
  CreateWindowEx =
      (CreateWindowEx_t)GetProcAddress(user32_dll, "CreateWindowExA");
  AdjustWindowRect =
      (AdjustWindowRect_t)GetProcAddress(user32_dll, "AdjustWindowRect");
  wsprintf = (wsprintf_t)GetProcAddress(user32_dll, "wsprintfA");
  SetWindowText = (SetWindowText_t)GetProcAddress(user32_dll, "SetWindowTextA");
  SetProcessDPIAware =
      (SetProcessDPIAware_t)GetProcAddress(user32_dll, "SetProcessDPIAware");
  MessageBox = (MessageBox_t)GetProcAddress(user32_dll, "MessageBoxA");
  GetClientRect = (GetClientRect_t)GetProcAddress(user32_dll, "GetClientRect");
}

#undef MZ_TINYWIN_IMPLEMENTATION
#endif // #ifdef MZ_TINYWIN_IMPLEMENTATION
