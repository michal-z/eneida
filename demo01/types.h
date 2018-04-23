#pragma once

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
typedef float f32;
typedef double f64;
#define float __INVALID__
#define double __INVALID__
#define unsigned __INVALID__
#define int __INVALID__
#define short __INVALID__
#define long __INVALID__

typedef struct D3D12_CPU_DESCRIPTOR_HANDLE {
    u64 ptr;
} D3D12_CPU_DESCRIPTOR_HANDLE;

typedef struct D3D12_GPU_DESCRIPTOR_HANDLE {
    u64 ptr;
} D3D12_GPU_DESCRIPTOR_HANDLE;

typedef struct ID3D12Device ID3D12Device;
typedef struct ID3D12Resource ID3D12Resource;
typedef struct ID3D12CommandQueue ID3D12CommandQueue;
typedef struct ID3D12CommandAllocator ID3D12CommandAllocator;
typedef struct ID3D12GraphicsCommandList ID3D12GraphicsCommandList;
typedef struct IDXGISwapChain3 IDXGISwapChain3;
typedef enum D3D12_HEAP_TYPE D3D12_HEAP_TYPE;
typedef enum D3D_FEATURE_LEVEL D3D_FEATURE_LEVEL;
