#pragma once
#include "types.h"

typedef struct renderer {
    ID3D12Device *d3d;
    ID3D12CommandQueue *cmdqueue;
    ID3D12CommandAllocator *cmdalloc[2];
    ID3D12GraphicsCommandList *cmdlist;
    u32 frame_index;
    u32 back_buffer_index;
    u32 descriptor_size;
    u32 descriptor_size_rtv;
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_heap_start;
    D3D12_CPU_DESCRIPTOR_HANDLE dsv_heap_start;
    ID3D12Resource *color_buffers[4];
    ID3D12Resource *depth_buffer;
    void *priv;
} renderer_t;

renderer_t *gr_init(void *window);
void gr_shutdown(renderer_t *rend);
void gr_present_frame(renderer_t *rend);
void gr_flush(renderer_t *rend);
