#ifndef MZ_DIRECT3D12_INCLUDED__
#define MZ_DIRECT3D12_INCLUDED__

#include "mz_windows.h"

#define MZD3D_FAILED(hr) ((hr) < 0)
#define MZD3D_VHR(hr) \
    if ((hr) < 0) {   \
        assert(0);    \
    }
#define MZD3D_RELEASE(obj)           \
    if ((obj)) {                     \
        (obj)->vtbl->Release((obj)); \
        (obj) = NULL;                \
    }

typedef struct {
    u16 index;
    u16 generation;
} mzd3d_resource_handle;

typedef struct {
    u16 index;
    u16 generation;
} mzd3d_pipeline_handle;

typedef struct mzd3d_context mzd3d_context;

mzd3d_context *mzd3d_create_context(void *window);
void mzd3d_destroy_context(mzd3d_context *gfx);

D3D12_CPU_DESCRIPTOR_HANDLE mzd3d_allocate_cpu_descriptors(mzd3d_context *gfx, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 count);
void mzd3d_allocate_gpu_descriptors(mzd3d_context *gfx, u32 count, D3D12_CPU_DESCRIPTOR_HANDLE *out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE *out_gpu_handle);
D3D12_GPU_DESCRIPTOR_HANDLE mzd3d_copy_descriptors_to_gpu_heap(mzd3d_context *gfx, u32 count, D3D12_CPU_DESCRIPTOR_HANDLE src_base_handle);

void mzd3d_allocate_upload_memory(mzd3d_context *gfx, u32 size, u8 **out_cpu_addr, D3D12_GPU_VIRTUAL_ADDRESS *out_gpu_addr);
void mzd3d_allocate_upload_buffer_region(mzd3d_context *gfx, u32 size, u8 **out_cpu_addr, ID3D12Resource **out_buffer, u64 *out_buffer_offset);

mzd3d_resource_handle mzd3d_create_committed_resource(mzd3d_context *gfx, D3D12_HEAP_TYPE heap_type, D3D12_HEAP_FLAGS heap_flags, const D3D12_RESOURCE_DESC *desc, D3D12_RESOURCE_STATES initial_state, const D3D12_CLEAR_VALUE *clear_value);
void mzd3d_destroy_resource(mzd3d_context *gfx, mzd3d_resource_handle handle);
ID3D12Resource *mzd3d_get_resource(mzd3d_context *gfx, mzd3d_resource_handle handle);

ID3D12Device *mzd3d_get_device(mzd3d_context *gfx);
ID3D12GraphicsCommandList *mzd3d_get_command_list(mzd3d_context *gfx);

void mzd3d_get_back_buffer(mzd3d_context *gfx, mzd3d_resource_handle *out_handle, D3D12_CPU_DESCRIPTOR_HANDLE *out_rtv);
void mzd3d_get_depth_stencil_buffer(mzd3d_context *gfx, mzd3d_resource_handle *out_handle, D3D12_CPU_DESCRIPTOR_HANDLE *out_dsv);

ID3D12GraphicsCommandList *mzd3d_begin_frame(mzd3d_context *gfx);
void mzd3d_end_frame(mzd3d_context *gfx, u32 swap_interval);

void mzd3d_wait_for_gpu(mzd3d_context *gfx);

void mzd3d_transition_barrier(mzd3d_context *gfx, mzd3d_resource_handle handle, D3D12_RESOURCE_STATES state_after);

#endif // #ifndef MZ_DIRECT3D12_INCLUDED__

//
//
// Stop now, if you are only interested in the API.
// Below, you find the implementation.
//
//

#ifdef __INTELLISENSE__
// This makes MSVC intellisense work.
#define MZ_DIRECT3D12_IMPLEMENTATION
#endif

#ifdef MZ_DIRECT3D12_IMPLEMENTATION

#include "mz_libc.h"
#include "mz_library.h"

#define _MZD3D_MAX_NUM_RESOURCES 256
#define _MZD3D_MAX_NUM_PIPELINES 128
#define _MZD3D_NUM_FRAMES 2

typedef struct {
    ID3D12Resource *raw;
    D3D12_RESOURCE_STATES state;
    DXGI_FORMAT format;
} _mzd3d_resource;

typedef struct {
    ID3D12PipelineState *pso;
    ID3D12RootSignature *rs;
} _mzd3d_pipeline;

typedef struct {
    ID3D12DescriptorHeap *heap;
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_start;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_start;
    u32 size;
    u32 capacity;
    u32 descriptor_size;
} _mzd3d_descriptor_heap;

typedef struct {
    ID3D12Resource *heap;
    u8 *cpu_start;
    D3D12_GPU_VIRTUAL_ADDRESS gpu_start;
    u32 size;
    u32 capacity;
} _mzd3d_gpu_memory_heap;

typedef struct {
    _mzd3d_resource *resources;
    u16 *generations;
} _mzd3d_resource_pool;

typedef struct {
    _mzd3d_pipeline *pipelines;
    u16 *generations;
} _mzd3d_pipeline_pool;

struct mzd3d_context {
    ID3D12Device *device;
    ID3D12GraphicsCommandList *cmd_list;
    u32 resolution[2];
    u32 frame_index;
    ID3D12CommandQueue *cmd_queue;
    _mzd3d_descriptor_heap rtv_heap;
    _mzd3d_descriptor_heap dsv_heap;
    _mzd3d_descriptor_heap cbv_srv_uav_cpu_heap;
    _mzd3d_descriptor_heap cbv_srv_uav_gpu_heaps[_MZD3D_NUM_FRAMES];
    _mzd3d_gpu_memory_heap upload_memory_heaps[_MZD3D_NUM_FRAMES];
    _mzd3d_resource_pool resource_pool;
    _mzd3d_pipeline_pool pipeline_pool;
    void *window;
    ID3D12CommandAllocator *cmd_alloc[_MZD3D_NUM_FRAMES];
    u32 back_buffer_index;
    IDXGISwapChain3 *swap_chain;
    mzd3d_resource_handle back_buffers[4];
    mzd3d_resource_handle depth_stencil_buffer;
    ID3D12Fence *frame_fence;
    void *frame_fence_event;
    u64 num_frames;
};

ID3D12Device *mzd3d_get_device(mzd3d_context *gfx)
{
    assert(gfx);
    return gfx->device;
}

ID3D12GraphicsCommandList *mzd3d_get_command_list(mzd3d_context *gfx)
{
    assert(gfx);
    return gfx->cmd_list;
}

static D3D12_CPU_DESCRIPTOR_HANDLE _mzd3d_allocate_cpu_descriptors(_mzd3d_descriptor_heap *dh, u32 count)
{
    assert(dh && (dh->size + count) < dh->capacity);

    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
    cpu_handle.ptr = dh->cpu_start.ptr + (u64)dh->size * dh->descriptor_size;
    dh->size += count;

    return cpu_handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE mzd3d_allocate_cpu_descriptors(mzd3d_context *gfx, D3D12_DESCRIPTOR_HEAP_TYPE type, u32 count)
{
    assert(gfx);
    if (type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV) {
        return _mzd3d_allocate_cpu_descriptors(&gfx->rtv_heap, count);
    } else if (type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV) {
        return _mzd3d_allocate_cpu_descriptors(&gfx->dsv_heap, count);
    } else if (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) {
        return _mzd3d_allocate_cpu_descriptors(&gfx->cbv_srv_uav_cpu_heap, count);
    }
    assert(0);
    return (D3D12_CPU_DESCRIPTOR_HANDLE) { 0 };
}

static _mzd3d_descriptor_heap _mzd3d_create_descriptor_heap(ID3D12Device *device, u32 capacity, D3D12_DESCRIPTOR_HEAP_TYPE type, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
    _mzd3d_descriptor_heap dh = { 0 };
    dh.capacity = capacity;

    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = { 0 };
    heap_desc.NumDescriptors = capacity;
    heap_desc.Type = type;
    heap_desc.Flags = flags;

    MZD3D_VHR(device->vtbl->CreateDescriptorHeap(device, &heap_desc, &IID_ID3D12DescriptorHeap, (void **)&dh.heap));

    dh.heap->vtbl->GetCPUDescriptorHandleForHeapStart(dh.heap, &dh.cpu_start);
    if (flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE) {
        dh.heap->vtbl->GetGPUDescriptorHandleForHeapStart(dh.heap, &dh.gpu_start);
    }

    dh.descriptor_size = device->vtbl->GetDescriptorHandleIncrementSize(device, type);
    return dh;
}

static _mzd3d_gpu_memory_heap _mzd3d_create_gpu_memory_heap(ID3D12Device *device, u32 capacity, D3D12_HEAP_TYPE type)
{
    _mzd3d_gpu_memory_heap mh = { 0 };
    mh.capacity = capacity;

    const D3D12_RESOURCE_DESC buffer_desc = {
        .Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
        .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR,
        .Width = capacity,
        .Height = 1,
        .DepthOrArraySize = 1,
        .MipLevels = 1,
        .SampleDesc.Count = 1,
    };
    MZD3D_VHR(device->vtbl->CreateCommittedResource(device, &(D3D12_HEAP_PROPERTIES) { .Type = type }, D3D12_HEAP_FLAG_NONE, &buffer_desc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, &IID_ID3D12Resource, (void **)&mh.heap));

    mh.heap->vtbl->Map(mh.heap, 0, &(D3D12_RANGE) { 0, 0 }, (void **)&mh.cpu_start);
    mh.gpu_start = mh.heap->vtbl->GetGPUVirtualAddress(mh.heap);

    return mh;
}

static mzd3d_resource_handle _mzd3d_add_resource(_mzd3d_resource_pool *pool, ID3D12Resource *raw, D3D12_RESOURCE_STATES initial_state, DXGI_FORMAT format)
{
    assert(pool && raw);

    u32 slot_idx = 0;
    for (u32 i = 1; i <= _MZD3D_MAX_NUM_RESOURCES; ++i) {
        if (pool->resources[i].raw == NULL) {
            slot_idx = i;
            break;
        }
    }
    assert(slot_idx > 0 && slot_idx <= _MZD3D_MAX_NUM_RESOURCES);

    pool->resources[slot_idx].raw = raw;
    pool->resources[slot_idx].state = initial_state;
    pool->resources[slot_idx].format = format;

    return (mzd3d_resource_handle) { (u16)slot_idx, pool->generations[slot_idx] += 1 };
}

static mzd3d_pipeline_handle _mzd3d_add_pipeline(_mzd3d_pipeline_pool *pool, ID3D12PipelineState *pso, ID3D12RootSignature *rs)
{
    assert(pool && pso && rs);

    u32 slot_idx = 0;
    for (u32 i = 1; i <= _MZD3D_MAX_NUM_PIPELINES; ++i) {
        if (pool->pipelines[i].pso == NULL) {
            slot_idx = i;
            break;
        }
    }
    assert(slot_idx > 0 && slot_idx <= _MZD3D_MAX_NUM_PIPELINES);

    pool->pipelines[slot_idx].pso = pso;
    pool->pipelines[slot_idx].rs = rs;

    return (mzd3d_pipeline_handle) { (u16)slot_idx, pool->generations[slot_idx] += 1 };
}

ID3D12Resource *mzd3d_get_resource(mzd3d_context *gfx, mzd3d_resource_handle handle)
{
    assert(handle.index > 0 && handle.index <= _MZD3D_MAX_NUM_RESOURCES);
    assert(handle.generation > 0 && handle.generation == gfx->resource_pool.generations[handle.index]);
    assert(gfx->resource_pool.resources[handle.index].raw);
    return gfx->resource_pool.resources[handle.index].raw;
}

_mzd3d_resource *_mzd3d_get_resource_state(mzd3d_context *gfx, mzd3d_resource_handle handle)
{
    assert(handle.index > 0 && handle.index <= _MZD3D_MAX_NUM_RESOURCES);
    assert(handle.generation > 0 && handle.generation == gfx->resource_pool.generations[handle.index]);
    assert(gfx->resource_pool.resources[handle.index].raw);
    return &gfx->resource_pool.resources[handle.index];
}

mzd3d_context *mzd3d_create_context(void *window)
{
    assert(window);
    mzd3d_context *gfx = MZL_MALLOC(sizeof(mzd3d_context));
    memset(gfx, 0, sizeof(mzd3d_context));

    IDXGIFactory4 *factory;
#ifdef _DEBUG
    MZD3D_VHR(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, &IID_IDXGIFactory4, (void **)&factory));
#else
    MZD3D_VHR(CreateDXGIFactory2(0, &IID_IDXGIFactory4, (void **)&factory));
#endif
#ifdef _DEBUG
    {
        ID3D12Debug *dbg;
        D3D12GetDebugInterface(&IID_ID3D12Debug, (void **)&dbg);
        if (dbg) {
            dbg->vtbl->EnableDebugLayer(dbg);
            ID3D12Debug1 *dbg1;
            dbg->vtbl->QueryInterface(dbg, &IID_ID3D12Debug1, (void **)&dbg1);
            if (dbg1) {
                dbg1->vtbl->SetEnableGPUBasedValidation(dbg1, TRUE);
            }
            MZD3D_RELEASE(dbg);
            MZD3D_RELEASE(dbg1);
        }
    }
#endif
    if (D3D12CreateDevice(NULL, D3D_FEATURE_LEVEL_11_1, &IID_ID3D12Device, (void **)&gfx->device) < 0) {
        MessageBoxA(window, "This application requires Windows 10 (May 2019) or newer and GPU with raytracing support.", "D3D12CreateDevice failed", MB_OK | MB_ICONERROR);
        exit(0);
    }

    gfx->window = window;

    D3D12_COMMAND_QUEUE_DESC cmd_queue_desc = { 0 };
    cmd_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    cmd_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    cmd_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    MZD3D_VHR(gfx->device->vtbl->CreateCommandQueue(gfx->device, &cmd_queue_desc, &IID_ID3D12CommandQueue, (void **)&gfx->cmd_queue));

    DXGI_SWAP_CHAIN_DESC swap_chain_desc = { 0 };
    swap_chain_desc.BufferCount = 4;
    swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.OutputWindow = window;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.Windowed = TRUE;

    IDXGISwapChain *temp_swap_chain;
    MZD3D_VHR(factory->vtbl->CreateSwapChain(factory, (IUnknown *)gfx->cmd_queue, &swap_chain_desc, &temp_swap_chain));
    MZD3D_VHR(temp_swap_chain->vtbl->QueryInterface(temp_swap_chain, &IID_IDXGISwapChain3, (void **)&gfx->swap_chain));
    MZD3D_RELEASE(temp_swap_chain);
    MZD3D_RELEASE(factory);

    RECT rect;
    GetClientRect(window, &rect);
    gfx->resolution[0] = (u32)rect.right;
    gfx->resolution[1] = (u32)rect.bottom;

    for (u32 i = 0; i < _MZD3D_NUM_FRAMES; ++i) {
        MZD3D_VHR(gfx->device->vtbl->CreateCommandAllocator(gfx->device, D3D12_COMMAND_LIST_TYPE_DIRECT, &IID_ID3D12CommandAllocator, (void **)&gfx->cmd_alloc[i]));
    }

    gfx->rtv_heap = _mzd3d_create_descriptor_heap(gfx->device, 1024, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    gfx->dsv_heap = _mzd3d_create_descriptor_heap(gfx->device, 1024, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
    gfx->cbv_srv_uav_cpu_heap = _mzd3d_create_descriptor_heap(gfx->device, 16 * 1024, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);

    for (u32 i = 0; i < _MZD3D_NUM_FRAMES; ++i) {
        gfx->cbv_srv_uav_gpu_heaps[i] = _mzd3d_create_descriptor_heap(gfx->device, 16 * 1024, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
    }
    for (u32 i = 0; i < _MZD3D_NUM_FRAMES; ++i) {
        gfx->upload_memory_heaps[i] = _mzd3d_create_gpu_memory_heap(gfx->device, 8 * 1024 * 1024, D3D12_HEAP_TYPE_UPLOAD);
    }

    gfx->resource_pool.resources = MZL_MALLOC((_MZD3D_MAX_NUM_RESOURCES + 1) * sizeof(_mzd3d_resource));
    gfx->resource_pool.generations = MZL_MALLOC((_MZD3D_MAX_NUM_RESOURCES + 1) * sizeof(u16));
    memset(gfx->resource_pool.resources, 0, (_MZD3D_MAX_NUM_RESOURCES + 1) * sizeof(_mzd3d_resource));
    memset(gfx->resource_pool.generations, 0, (_MZD3D_MAX_NUM_RESOURCES + 1) * sizeof(u16));

    gfx->pipeline_pool.pipelines = MZL_MALLOC((_MZD3D_MAX_NUM_PIPELINES + 1) * sizeof(_mzd3d_pipeline));
    gfx->pipeline_pool.generations = MZL_MALLOC((_MZD3D_MAX_NUM_PIPELINES + 1) * sizeof(u16));
    memset(gfx->pipeline_pool.pipelines, 0, (_MZD3D_MAX_NUM_PIPELINES + 1) * sizeof(_mzd3d_pipeline));
    memset(gfx->pipeline_pool.generations, 0, (_MZD3D_MAX_NUM_PIPELINES + 1) * sizeof(u16));

    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = mzd3d_allocate_cpu_descriptors(gfx, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 4);

        for (u32 i = 0; i < 4; ++i) {
            ID3D12Resource *buffer;
            MZD3D_VHR(gfx->swap_chain->vtbl->GetBuffer(gfx->swap_chain, i, &IID_ID3D12Resource, (void **)&buffer));
            gfx->back_buffers[i] = _mzd3d_add_resource(&gfx->resource_pool, buffer, D3D12_RESOURCE_STATE_PRESENT, swap_chain_desc.BufferDesc.Format);
            gfx->device->vtbl->CreateRenderTargetView(gfx->device, mzd3d_get_resource(gfx, gfx->back_buffers[i]), NULL, handle);
            handle.ptr += gfx->rtv_heap.descriptor_size;
        }
    }

    {
        const D3D12_RESOURCE_DESC image_desc = {
            .Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
            .Format = DXGI_FORMAT_D32_FLOAT,
            .Width = gfx->resolution[0],
            .Height = gfx->resolution[1],
            .DepthOrArraySize = 1,
            .MipLevels = 1,
            .SampleDesc.Count = 1,
            .Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE,
        };
        gfx->depth_stencil_buffer = mzd3d_create_committed_resource(gfx, D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, &image_desc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &(D3D12_CLEAR_VALUE) { .Format = DXGI_FORMAT_D32_FLOAT, .DepthStencil = { 1.0f, 0 } });

        D3D12_CPU_DESCRIPTOR_HANDLE handle = mzd3d_allocate_cpu_descriptors(gfx, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);

        const D3D12_DEPTH_STENCIL_VIEW_DESC view_desc = {
            .Format = DXGI_FORMAT_D32_FLOAT,
            .ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D,
            .Flags = D3D12_DSV_FLAG_NONE,
        };
        gfx->device->vtbl->CreateDepthStencilView(gfx->device, mzd3d_get_resource(gfx, gfx->depth_stencil_buffer), &view_desc, handle);
    }

    MZD3D_VHR(gfx->device->vtbl->CreateCommandList(gfx->device, 0, D3D12_COMMAND_LIST_TYPE_DIRECT, gfx->cmd_alloc[0], NULL, &IID_ID3D12GraphicsCommandList, (void **)&gfx->cmd_list));
    MZD3D_VHR(gfx->cmd_list->vtbl->Close(gfx->cmd_list));

    MZD3D_VHR(gfx->device->vtbl->CreateFence(gfx->device, 0, D3D12_FENCE_FLAG_NONE, &IID_ID3D12Fence, (void **)&gfx->frame_fence));
    gfx->frame_fence_event = CreateEventExA(NULL, NULL, 0, EVENT_ALL_ACCESS);

    mzd3d_begin_frame(gfx);

    return gfx;
}

mzd3d_resource_handle mzd3d_create_committed_resource(mzd3d_context *gfx, D3D12_HEAP_TYPE heap_type, D3D12_HEAP_FLAGS heap_flags, const D3D12_RESOURCE_DESC *desc, D3D12_RESOURCE_STATES initial_state, const D3D12_CLEAR_VALUE *clear_value)
{
    assert(gfx);
    ID3D12Resource *raw;
    MZD3D_VHR(gfx->device->vtbl->CreateCommittedResource(gfx->device, &(D3D12_HEAP_PROPERTIES) { .Type = heap_type }, heap_flags, desc, initial_state, clear_value, &IID_ID3D12Resource, (void **)&raw));
    return _mzd3d_add_resource(&gfx->resource_pool, raw, initial_state, desc->Format);
}

void mzd3d_destroy_context(mzd3d_context *gfx)
{
    assert(gfx);
    for (u32 i = 1; i <= _MZD3D_MAX_NUM_RESOURCES; ++i) {
        MZD3D_RELEASE(gfx->resource_pool.resources[i].raw);
    }
    MZL_FREE(gfx->resource_pool.resources);
    MZL_FREE(gfx->resource_pool.generations);

    for (u32 i = 1; i <= _MZD3D_MAX_NUM_PIPELINES; ++i) {
        MZD3D_RELEASE(gfx->pipeline_pool.pipelines[i].pso);
        MZD3D_RELEASE(gfx->pipeline_pool.pipelines[i].rs);
    }
    MZL_FREE(gfx->pipeline_pool.pipelines);
    MZL_FREE(gfx->pipeline_pool.generations);

    MZD3D_RELEASE(gfx->rtv_heap.heap);
    MZD3D_RELEASE(gfx->dsv_heap.heap);
    MZD3D_RELEASE(gfx->cbv_srv_uav_cpu_heap.heap);
    for (u32 i = 0; i < _MZD3D_NUM_FRAMES; ++i) {
        MZD3D_RELEASE(gfx->cbv_srv_uav_gpu_heaps[i].heap);
    }
    for (u32 i = 0; i < _MZD3D_NUM_FRAMES; ++i) {
        MZD3D_RELEASE(gfx->upload_memory_heaps[i].heap);
    }

    for (u32 i = 0; i < _MZD3D_NUM_FRAMES; ++i) {
        MZD3D_RELEASE(gfx->cmd_alloc[i]);
    }
    MZD3D_RELEASE(gfx->swap_chain);
    if (gfx->frame_fence_event) {
        CloseHandle(gfx->frame_fence_event);
    }
    MZD3D_RELEASE(gfx->frame_fence);
    MZD3D_RELEASE(gfx->cmd_queue);
    MZD3D_RELEASE(gfx->cmd_list);
    MZD3D_RELEASE(gfx->device);
}

ID3D12GraphicsCommandList *mzd3d_begin_frame(mzd3d_context *gfx)
{
    ID3D12CommandAllocator *cmdalloc = gfx->cmd_alloc[gfx->frame_index];
    ID3D12GraphicsCommandList *cmdlist = gfx->cmd_list;
    cmdalloc->vtbl->Reset(cmdalloc);
    cmdlist->vtbl->Reset(cmdlist, cmdalloc, NULL);
    cmdlist->vtbl->SetDescriptorHeaps(cmdlist, 1, &gfx->cbv_srv_uav_gpu_heaps[gfx->frame_index].heap);
    return cmdlist;
}

void mzd3d_end_frame(mzd3d_context *gfx, u32 swap_interval)
{
    MZD3D_VHR(gfx->cmd_list->vtbl->Close(gfx->cmd_list));
    gfx->cmd_queue->vtbl->ExecuteCommandLists(gfx->cmd_queue, 1, (ID3D12CommandList **)&gfx->cmd_list);

    gfx->swap_chain->vtbl->Present(gfx->swap_chain, swap_interval, 0);
    gfx->cmd_queue->vtbl->Signal(gfx->cmd_queue, gfx->frame_fence, ++gfx->num_frames);

    const u64 num_gpu_frames = gfx->frame_fence->vtbl->GetCompletedValue(gfx->frame_fence);

    if ((gfx->num_frames - num_gpu_frames) >= _MZD3D_NUM_FRAMES) {
        gfx->frame_fence->vtbl->SetEventOnCompletion(gfx->frame_fence, num_gpu_frames + 1, gfx->frame_fence_event);
        WaitForSingleObject(gfx->frame_fence_event, INFINITE);
    }

    gfx->frame_index = (gfx->frame_index + 1) % _MZD3D_NUM_FRAMES;
    gfx->back_buffer_index = gfx->swap_chain->vtbl->GetCurrentBackBufferIndex(gfx->swap_chain);
    gfx->cbv_srv_uav_gpu_heaps[gfx->frame_index].size = 0;
    gfx->upload_memory_heaps[gfx->frame_index].size = 0;
}

void mzd3d_wait_for_gpu(mzd3d_context *gfx)
{
    gfx->cmd_queue->vtbl->Signal(gfx->cmd_queue, gfx->frame_fence, ++gfx->num_frames);
    gfx->frame_fence->vtbl->SetEventOnCompletion(gfx->frame_fence, gfx->num_frames, gfx->frame_fence_event);
    WaitForSingleObject(gfx->frame_fence_event, INFINITE);

    gfx->cbv_srv_uav_gpu_heaps[gfx->frame_index].size = 0;
    gfx->upload_memory_heaps[gfx->frame_index].size = 0;
}

void mzd3d_destroy_resource(mzd3d_context *gfx, mzd3d_resource_handle handle)
{
    _mzd3d_resource *resource = _mzd3d_get_resource_state(gfx, handle);

    u32 refcount = resource->raw->vtbl->Release(resource->raw);
    assert(refcount == 0);

    resource->raw = NULL;
    resource->state = D3D12_RESOURCE_STATE_COMMON;
    resource->format = DXGI_FORMAT_UNKNOWN;
}

void mzd3d_transition_barrier(mzd3d_context *gfx, mzd3d_resource_handle handle, D3D12_RESOURCE_STATES state_after)
{
    _mzd3d_resource *resource = _mzd3d_get_resource_state(gfx, handle);

    if (state_after != resource->state) {
        // NOTE(mziulek): Buffer barriers and submit all at once? Support subresource granularity?
        gfx->cmd_list->vtbl->ResourceBarrier(
            gfx->cmd_list,
            1,
            &(D3D12_RESOURCE_BARRIER) {
                .Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                .Transition.pResource = resource->raw,
                .Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                .Transition.StateBefore = resource->state,
                .Transition.StateAfter = state_after });
        resource->state = state_after;
    }
}

void mzd3d_get_back_buffer(mzd3d_context *gfx, mzd3d_resource_handle *out_handle, D3D12_CPU_DESCRIPTOR_HANDLE *out_rtv)
{
    assert(out_handle && out_rtv);
    *out_handle = gfx->back_buffers[gfx->back_buffer_index];
    *out_rtv = gfx->rtv_heap.cpu_start;
    out_rtv->ptr += (u64)gfx->back_buffer_index * gfx->rtv_heap.descriptor_size;
    assert(mzd3d_get_resource(gfx, *out_handle));
}

void mzd3d_get_depth_stencil_buffer(mzd3d_context *gfx, mzd3d_resource_handle *out_handle, D3D12_CPU_DESCRIPTOR_HANDLE *out_dsv)
{
    assert(out_handle && out_dsv);
    *out_handle = gfx->depth_stencil_buffer;
    *out_dsv = gfx->dsv_heap.cpu_start;
    assert(mzd3d_get_resource(gfx, *out_handle));
}

void mzd3d_allocate_gpu_descriptors(mzd3d_context *gfx, u32 count, D3D12_CPU_DESCRIPTOR_HANDLE *out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE *out_gpu_handle)
{
    assert(out_cpu_handle && out_gpu_handle);

    _mzd3d_descriptor_heap *dh = &gfx->cbv_srv_uav_gpu_heaps[gfx->frame_index];

    assert((dh->size + count) < dh->capacity);

    out_cpu_handle->ptr = dh->cpu_start.ptr + (u64)dh->size * dh->descriptor_size;
    out_gpu_handle->ptr = dh->gpu_start.ptr + (u64)dh->size * dh->descriptor_size;

    dh->size += count;
}

D3D12_GPU_DESCRIPTOR_HANDLE mzd3d_copy_descriptors_to_gpu_heap(mzd3d_context *gfx, u32 count, D3D12_CPU_DESCRIPTOR_HANDLE src_base_handle)
{
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_base_handle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_base_handle;
    mzd3d_allocate_gpu_descriptors(gfx, count, &cpu_base_handle, &gpu_base_handle);
    gfx->device->vtbl->CopyDescriptorsSimple(gfx->device, count, cpu_base_handle, src_base_handle, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    return gpu_base_handle;
}

static void _mzd3d_allocate_gpu_memory(_mzd3d_gpu_memory_heap *mheap, u32 size, u8 **out_cpu_addr, D3D12_GPU_VIRTUAL_ADDRESS *out_gpu_addr)
{
    assert(mheap && size > 0 && out_cpu_addr && out_gpu_addr);

    if (size & 0xff) {
        // Always align to 256 bytes.
        size = (size + 255) & ~0xff;
    }
    if ((mheap->size + size) >= mheap->capacity) {
        *out_cpu_addr = NULL;
        *out_gpu_addr = 0;
        return;
    }

    *out_cpu_addr = mheap->cpu_start + mheap->size;
    *out_gpu_addr = mheap->gpu_start + mheap->size;

    mheap->size += size;
}

void mzd3d_allocate_upload_memory(mzd3d_context *gfx, u32 size, u8 **out_cpu_addr, D3D12_GPU_VIRTUAL_ADDRESS *out_gpu_addr)
{
    assert(gfx && size > 0 && out_cpu_addr && out_gpu_addr);

    u8 *cpu_addr;
    D3D12_GPU_VIRTUAL_ADDRESS gpu_addr;
    _mzd3d_allocate_gpu_memory(&gfx->upload_memory_heaps[gfx->frame_index], size, &cpu_addr, &gpu_addr);

    if (cpu_addr == NULL && gpu_addr == 0) {
        MZD3D_VHR(gfx->cmd_list->vtbl->Close(gfx->cmd_list));
        gfx->cmd_queue->vtbl->ExecuteCommandLists(gfx->cmd_queue, 1, (ID3D12CommandList **)&gfx->cmd_list);
        mzd3d_wait_for_gpu(gfx);
        mzd3d_begin_frame(gfx);
    }

    _mzd3d_allocate_gpu_memory(&gfx->upload_memory_heaps[gfx->frame_index], size, &cpu_addr, &gpu_addr);
    assert(cpu_addr != NULL && gpu_addr != 0);

    *out_cpu_addr = cpu_addr;
    *out_gpu_addr = gpu_addr;
}

void mzd3d_allocate_upload_buffer_region(mzd3d_context *gfx, u32 size, u8 **out_cpu_addr, ID3D12Resource **out_buffer, u64 *out_buffer_offset)
{
    assert(gfx && size > 0 && out_cpu_addr && out_buffer && out_buffer_offset);

    if (size & 0xff) {
        // Always align to 256 bytes.
        size = (size + 255) & ~0xff;
    }

    u8 *cpu_addr;
    D3D12_GPU_VIRTUAL_ADDRESS gpu_addr;
    mzd3d_allocate_upload_memory(gfx, size, &cpu_addr, &gpu_addr);

    *out_cpu_addr = cpu_addr;
    *out_buffer = gfx->upload_memory_heaps[gfx->frame_index].heap;
    *out_buffer_offset = gfx->upload_memory_heaps[gfx->frame_index].size - size;
}

#endif // #ifdef MZ_DIRECT3D12_IMPLEMENTATION
