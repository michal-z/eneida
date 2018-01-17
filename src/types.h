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

typedef struct ID3D12Device ID3D12Device;
typedef struct ID3D12CommandQueue ID3D12CommandQueue;
typedef struct IDXGISwapChain3 IDXGISwapChain3;
